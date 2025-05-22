#include "usbdevice.h"
#include "iomanager.h"
#include "usbinfo.h"
#include "src/utils/hostinfo/hostinfo.h"
#include "src/utils/logger/logger.h"
#include <qdebug.h>


namespace ET {
UsbDevice::UsbDevice(libusb_device *device, libusb_context *ctx, uint16_t productId, uint16_t vendorId,
                     uint16_t classId, QObject *parent)
        : QObject(parent),
          device(device),
          ctx(ctx),
          productId(productId),
          vendorId(vendorId),
          classId(classId) {
    cpuId = HostInfo::getCpuId();
    mac = HostInfo::getMac();
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
        } else {
            LOG_ERROR("Open Device Error!");
            return;
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
        checkOperationRes(libusb_set_auto_detach_kernel_driver(deviceHandle, 1), DETACH_KERNEL);
        checkOperationRes(libusb_set_configuration(deviceHandle, 1), SET_CONFIGURATION);
        checkOperationRes(libusb_claim_interface(deviceHandle, 0), CLAIM_INTERFACE);
        stateError = false;
    }
}

bool UsbDevice::isOpened() const {
    LOG_INFO("isOpened: devValid=", devValid);
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

void UsbDevice::onErrorOccurred(const QString &errStr, int errCode) {
    emit information(errStr, errCode);
}

void UsbDevice::onTransferFinished(IOType ioType, QByteArray data, int cmdType) {
    stateError = false;
    QString res = QString::fromLatin1(data);
    switch (cmdType) {
        case 1: {
            emit readKeyFinished(res);
            break;
        }
        case 2: {
            emit generateKeyFinished(res);
            break;
        }
        case 3: {
            emit readIdFinished(res);
            break;
        }
        default:
            Q_ASSERT(false);
    }
    Q_ASSERT(isIoBusy);
    cmdCache.removeFirst();
    if (!cmdCache.isEmpty()) {
        doSyncInterIo(1, IOType::WRITE, cmdCache.front().first, cmdCache.front().second);
    } else {
        isIoBusy = false;
    }
}

void UsbDevice::onTimeOut(IOType ioType) {
}

void UsbDevice::getKeyNonBlock() {
    doIoNonBlock(1);
}

void UsbDevice::getIdNonBlock() {
    doIoNonBlock(3);
}

void UsbDevice::generateKeyNonBlock(unsigned long sk) {
    doIoNonBlock(2, sk);
}

QString UsbDevice::refreshKeyBlocked(unsigned long sk) {
    key.clear();
    return doIoBlocked(1, sk);
}

QString UsbDevice::getKeyBlocked() {
    return doIoBlocked(0);
}

QString UsbDevice::doIoBlocked(int ioType, unsigned long sk) {
    QString res;
    BlockIoContext context(res);
    ProtocolCodecEngine tmpCodecEngine;
    tmpCodecEngine.frameDeclare("H(AAFF)S1CV(CRC16)E(AF)");
    tmpCodecEngine.setVerifyFlags("HSC");

    QByteArray buf, codeData;
    buf.fill(0, 64);

    // 0 Read Key 1 Refresh Key 2 Read Id
    switch (ioType) {
        case 0: {
            key.clear();
            tmpCodecEngine.registerType<ReadKey, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<ReadKeyFd>>(&context, &BlockIoContext::onGetKeyFinished);
            ReadKey data;
            data.cpuId = HostInfo::getCpuId();
            data.mac = HostInfo::getMac();
            codeData = tmpCodecEngine.encode<ReadKey>(data);
            break;
        }
        case 1: {
            key.clear();
            tmpCodecEngine.registerType<GenKey, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<GenKeyFd>>(&context, &BlockIoContext::onRefreshKeyFinished);
            GenKey data;
            data.cpuId = HostInfo::getCpuId();
            data.mac = HostInfo::getMac();
            data.sk = QString::number(sk);
            codeData = tmpCodecEngine.encode<GenKey>(data);
            LOG_INFO("data.cpuId=",data.cpuId,"data.mac=",data.mac,"data.sk=", data.sk, "CodeData=", QString(codeData.toHex()));
//            qDebug() << "Code Data: " << QString(codeData.toHex());
            break;
        }
        case 2: {
            tmpCodecEngine.registerType<ReadId, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<ReadIdFd>>(&context, &BlockIoContext::onReadIdFinished);
            ReadId data;
            codeData = tmpCodecEngine.encode<ReadId>(data);
            break;
        }
    }

    int dataSize = codeData.size(), transferred = 0, tmpTransferred = 0;
    while (transferred < dataSize) {
        buf.fill(0);
        memcpy(buf.data(), codeData.data() + transferred, qMin((dataSize - transferred), 64));
        auto transCode = libusb_interrupt_transfer(deviceHandle, CTL_POINT | LIBUSB_ENDPOINT_OUT,
                                                   (unsigned char *) buf.data(), INTER_SYNC_BUF_SIZE, &tmpTransferred,
                                                   2000);
        if (transCode != 0) {
            auto str = libusb_error_name(transCode);
            LOG_ERROR("Transfer Error:", str);
            return "";
        } else {
#ifdef OS_WIN
            transferred += (tmpTransferred - 1);
#elif defined(OS_LINUX)
            transferred += tmpTransferred;
#endif
        }
        LOG_INFO("Transferred:", transferred, " TempTeansferred:", tmpTransferred);
        QThread::msleep(3);
    }
//    LOG_INFO("Write Data:", QString(codeData.toHex()));
    int retry = 0;
    while (res.isEmpty()) {
        buf.fill(' ', 64);
        auto transCode = libusb_interrupt_transfer(deviceHandle,  CTL_POINT | LIBUSB_ENDPOINT_IN,
                                                   (unsigned char *) buf.data(), 64, &tmpTransferred, 2000);
        if (transCode != LIBUSB_SUCCESS) {
            auto str = libusb_error_name(transCode);
            LOG_ERROR("Read USB Error:", str);
            if(++retry > 3) {
                return "";
            }
        } else {
            LOG_INFO("Read USB Finish:", QString(buf.toHex()));
            tmpCodecEngine.appendBuffer(QByteArray::fromRawData((const char *) buf, 64));
        }
        QThread::msleep(3);
    }
    if (ioType != 2) {
        key = res;
    } else {
        id = res;
    }
    LOG_INFO("ioType:", ioType, "RES", res);
    return res;
}

QString UsbDevice::getIdBlocked() {
    return doIoBlocked(2);
}

void UsbDevice::doIoNonBlock(int cmdType, unsigned long sk, uint8_t endPoint) {
    cmdCache.removeAll({cmdType, sk});
    cmdCache.enqueue({cmdType, sk});
    if (!isIoBusy) {
        doSyncInterIo(endPoint, IOType::WRITE, cmdType, sk);
        isIoBusy = true;
    }
}

void UsbDevice::detachKernel() {
#ifdef OS_LINUX
//    struct usb_bus *bus;
//    struct usb_device *dev;
//
//    usb_init();
//    usb_find_busses();
//    usb_find_devices();
//    for (bus = usb_busses; bus; bus = bus->next) {
//        for (dev = bus->devices; dev; dev = dev->next) {
//            if(dev->descriptor.idVendor == VID && dev->descriptor.idProduct == PID) {
//                auto udev = usb_open(dev);
//                if(udev != nullptr) {
//                    LOG_INFO("USB Device Open Success!");
//                    LOG_INFO("usb_detach_kernel_driver_np: ", usb_detach_kernel_driver_np(udev, usedInterfaceNum));
//                    usb_close(udev);
//                    return;
//                } else {
//                    LOG_ERROR("USB Device Open Failed!");
//                    return;
//                }
//            }
//        }
//    }
#endif
}
}

