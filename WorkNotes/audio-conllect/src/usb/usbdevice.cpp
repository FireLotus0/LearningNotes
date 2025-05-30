#include "usbdevice.h"
#include "iomanager.h"
#include "usbinfo.h"

#include <qdebug.h>

UsbDevice::UsbDevice(libusb_device *device, libusb_context *ctx, uint16_t productId, uint16_t vendorId, uint16_t classId, QObject *parent)
    : QObject(parent),
      device(device),
      ctx(ctx),
      productId(productId),
      vendorId(vendorId),
      classId(classId) {
}

bool UsbDevice::checkOperationRes(int res, OPERATION op) {
    if (res != 0) {
        qInfo() << "Operation Failed: PID: " << productId << " VID:" << vendorId << op << libusb_error_name(res);
        return false;
    }
    devValid |= op;
    return true;
}

UsbDevice::~UsbDevice() {
    IoManager::instance().removeDevice(this);
    if (isOpened()) {
        if (devValid & CLAIM_INTERFACE) {
            libusb_release_interface(deviceHandle, usedInterfaceNum);
        }
        libusb_close(deviceHandle);
    }
}

void UsbDevice::openDevice() {
    if (isOpened()) {
        qWarning() << "Open Device Failed: Device has been open! ";
        return;
    }
    int res = LIBUSB_ERROR_NO_DEVICE;
    if (productId != LIBUSB_HOTPLUG_MATCH_ANY && vendorId != LIBUSB_HOTPLUG_MATCH_ANY) {
        deviceHandle = libusb_open_device_with_vid_pid(ctx, vendorId, productId);
        if (deviceHandle != nullptr) {
            res = 0;
            devValid |= OPEN_DEVICE;
        }
    } else {
        Q_ASSERT(device != nullptr);
        res = libusb_open(device, &deviceHandle);
        checkOperationRes(res, OPEN_DEVICE);
    }
    if (deviceHandle) {
        UsbInfo::resolveUsbInfo(this, productId, vendorId, classId);
#ifdef USE_ASYNC_IO
        IoManager::instance().startEvtHandle();
#endif
        // 注册回调函数
        checkOperationRes(libusb_set_auto_detach_kernel_driver(deviceHandle, 1), DETACH_KERNEL);
        checkOperationRes(libusb_set_configuration(deviceHandle, usedConfigNum), SET_CONFIGURATION);
        checkOperationRes(libusb_claim_interface(deviceHandle, usedInterfaceNum), CLAIM_INTERFACE);
        // 连接成功
        stateError = false;
        //        sampleTimer.start();
    }
}

bool UsbDevice::isOpened() const {
    return devValid != 0;
}
libusb_device_handle *UsbDevice::getDeviceHandle() {
    return deviceHandle;
}
void UsbDevice::insertEndPoint(uint8_t pointNumer, libusb_transfer_type transferType, IOType ioType) {
#ifdef USE_ASYNC_IO
    bool useAsync = true;
#else
    bool useAsync = false;
#endif
    auto &ioManager = IoManager::instance();
    IoManager::ThrType type;
    switch (transferType) {
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_CONTROL:
            type = useAsync ? IoManager::ASYNC_CTL_IO : IoManager::SYNC_CTL_IO;
            ioManager.insertTransfer(pointNumer, type, ioType, this);
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_BULK:
            ioManager.insertTransfer(pointNumer, IoManager::SYNC_BULK_IO, ioType, this);// 异步批量传输未定义
            type = IoManager::SYNC_BULK_IO;
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_INTERRUPT:
            type = useAsync ? IoManager::ASYNC_INTER_IO : IoManager::SYNC_INTER_IO;
            ioManager.insertTransfer(pointNumer, type, ioType, this);
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
            type = IoManager::ASYNC_ISO_IO;
            ioManager.insertTransfer(pointNumer, IoManager::ASYNC_ISO_IO, ioType, this);// 实时传输只有异步方式
            break;
        default:
            qWarning() << "Transfer Type Not Support!";
            Q_ASSERT(false);
    }
    pointTransType[pointNumer] = type;
}
bool UsbDevice::interfaceRequired(int configIndex, int interfaceIndex) const {
    return usedConfigNum == configIndex && usedInterfaceNum == interfaceIndex;
}
void UsbDevice::onErrorOccurred(const QString &errStr) {
    //    if (deviceHandle) {
    //        emit information(errStr, true);
    //    }
    qDebug() << "Error: " << errStr;
}

void UsbDevice::onTransferFinished(IOType ioType, QByteArray data) {
    stateError = false;
    if (ioType == IOType::WRITE) {
        writeNextCommand();
    } else {
        emit readFinished(data);
    }
}

void UsbDevice::read(uint8_t point) {
    Q_ASSERT(pointTransType.contains(point | LIBUSB_ENDPOINT_IN));
    switch (pointTransType[point | LIBUSB_ENDPOINT_IN]) {
        case IoManager::SYNC_CTL_IO: {
            uint8_t bmRequestType = 0;
            uint16_t wLength = CTL_BUF_SIZE;// 数据阶段传输阶段
            bmRequestType |= (1 << 7);      // device to host
            bmRequestType |= (2 << 4);      // vendor
            bmRequestType |= 2;             // specific point
            uint8_t bRequest = 0;
            uint16_t wValue = 0;
            uint16_t wIndex = 0;// 端点0
            doSyncCtlIO(bmRequestType, bRequest, wValue, wIndex, wLength, IOType::READ, QByteArray());
        } break;
        case IoManager::SYNC_INTER_IO: {
            doSyncInterIo(point, INTER_BUF_SIZE, IOType::READ, QByteArray());
//            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_BULK_IO: {
            doSyncBulkIO(point, BULK_BUF_SIZE, IOType::READ, QByteArray());
        } break;
        case IoManager::ASYNC_CTL_IO: {
            //            doAsyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_INTER_IO: {
            //            doAsyncInterIO(point, IOType::READ, QByteArray());
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_ISO_IO: {
            //            doAsyncIsoIO();
            Q_ASSERT(false);
        } break;
    }
}

void UsbDevice::onTimeOut(IOType ioType) {
    qDebug() << "Usb Time out! IO:" << (ioType == 0 ? "WRITE" : "READ");
    if (!stateError) {
        stateError = true;
        if (ioType == IOType::WRITE) {
            transfer(writeCmdCache.front(), DATA_POINT);
        } else {
            read(DATA_POINT);
        }
    }
}

void UsbDevice::writeNextCommand() {
    writeCmdCache.removeFirst();
    if (!writeCmdCache.isEmpty()) {
        transfer(writeCmdCache.front(), DATA_POINT);
    } else {
        isWriting = false;
    }
}

void UsbDevice::write(const QByteArray &data) {
    auto point = DATA_POINT;
    writeCmdCache.push_back(data);
    if (isWriting) {
        return;
    }
    transfer(data, point);
}

void UsbDevice::transfer(const QByteArray &data, uint8_t point) {
    Q_ASSERT(pointTransType.contains(point | LIBUSB_ENDPOINT_OUT));
    switch (pointTransType[point | LIBUSB_ENDPOINT_OUT]) {
        case IoManager::SYNC_CTL_IO: {
            //            doSyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_INTER_IO: {
            doSyncInterIo(point, INTER_BUF_SIZE, IOType::WRITE, data);
            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_BULK_IO: {
            doSyncBulkIO(point, data.size(), IOType::WRITE, data);
        } break;
        case IoManager::ASYNC_CTL_IO: {
            //            doAsyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_INTER_IO: {
            //            doAsyncInterIO(point, IOType::WRITE, data);
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_ISO_IO: {
            //            doAsyncIsoIO();
        } break;
    }
}
