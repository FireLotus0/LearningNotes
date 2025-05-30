#include "iohelper.h"

#include <qdebug.h>
#include <qthread.h>

AbstractIO::AbstractIO(QObject *parent) : QObject(parent) {
    qRegisterMetaType<IOType>();
}

bool AbstractIO::checkResult(int res, IOType ioType) {
    if(res == 0) {
        return true;
    } else if(res == LIBUSB_ERROR_TIMEOUT) {
        emit timeout(ioType);
    }
    else {
        auto str = libusb_error_name(res);
        qWarning() << "ERROR: " << str;
        emit errorOccured(str);
    }
    return false;
}

SyncIO::SyncIO(libusb_device_handle *deviceHandle, int buffSz, unsigned int timeout, QObject *parent)
    : AbstractIO(parent)
      , deviceHandle(deviceHandle)
      , timeout(timeout)
{
    rdBuffer.resize(buffSz);
    wrBuffer.resize(buffSz);
}
CtlIOSync::CtlIOSync(libusb_device_handle *dev_handle, int buffSz, unsigned int timeout, QObject *parent)
    : SyncIO(dev_handle, buffSz, timeout, parent)
{
}
void CtlIOSync::doTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data) {
    int res;
    if(ioType == IOType::WRITE) {
        memcpy(wrBuffer.data(), data.data(), CTL_BUF_SIZE);
        res = libusb_control_transfer(deviceHandle, bmRequestType, bRequest, wValue, wIndex, (unsigned char*)wrBuffer.data(), wLength, timeout);
    } else {
        rdBuffer.fill(0);
        res = libusb_control_transfer(deviceHandle, bmRequestType, bRequest, wValue, wIndex, (unsigned char*)rdBuffer.data(), wLength, timeout);
    }
    if(checkResult(res, ioType)) {
        if(ioType == IOType::WRITE) {
            emit finished(ioType, wrBuffer);
        } else {
            emit finished(ioType, rdBuffer);
        }
    }
}

BulkIOSync::BulkIOSync(IOType type, libusb_device_handle *dev_handle, int buffSz, unsigned int timeout, QObject *parent)
    : SyncIO(dev_handle, buffSz, timeout, parent)
    , ioType(type)
{
}
void BulkIOSync::doTransfer(unsigned char endpoint, int length, IOType type, const QByteArray& data)
{
    if(type != ioType) {
        return;
    }
    int res;
    int transferred = 0;
    if(ioType == IOType::WRITE) {
        memcpy(wrBuffer.data(), data.data(), length);
        res = libusb_bulk_transfer(deviceHandle, endpoint, (unsigned char*)wrBuffer.data(), length, &transferred, timeout);
    } else {
        rdBuffer.fill(0);
        res = libusb_bulk_transfer(deviceHandle, endpoint, (unsigned char*)rdBuffer.data(), length, &transferred, timeout);
    }
    if(checkResult(res, ioType)) {
        if(ioType == IOType::WRITE) {
            emit finished(ioType, wrBuffer);
        } else {
//            qInfo() << "read thread:" << QThread::currentThreadId();
            emit finished(ioType, rdBuffer);
//            qDebug() << "read data:" << rdBuffer;
        }
    }
}
InterruptIOSync::InterruptIOSync(libusb_device_handle *dev_handle, int buffSz, unsigned int timeout, QObject *parent)
    : SyncIO(dev_handle, buffSz, timeout, parent) {
}
void InterruptIOSync::doTransfer(unsigned char endpoint, int length, IOType ioType, const QByteArray& data) {
    if(ioType == IOType::WRITE) {
        int dataSize = data.size(), transferred = 0, tmpTransferred = 0;
        wrBuffer.resize(INTER_BUF_SIZE);
        while (transferred < dataSize) {
            wrBuffer.fill(0);
            memcpy(wrBuffer.data(), data.data() + transferred, qMin((dataSize - transferred), INTER_BUF_SIZE));
            auto transCode = libusb_interrupt_transfer(deviceHandle, endpoint | LIBUSB_ENDPOINT_OUT,
                                                       (unsigned char *) data.data(), INTER_BUF_SIZE, &tmpTransferred,
                                                       2000);
            if (transCode != LIBUSB_SUCCESS) {
                auto str = libusb_error_name(transCode);
                emit errorOccured(str);
                break;
            } else {
#ifdef Q_OS_WIN
                qDebug() << "transfer size:" << tmpTransferred;
                transferred += (tmpTransferred - 1);
#elif defined(OS_LINUX)
                transferred += tmpTransferred;
#endif
            }
            QThread::msleep(3);
        }
        if(transferred >= data.size()) {
            emit finished(IOType::WRITE, {});
        }
    } else {
        int tryTimes = 0;
        rdBuffer.clear();
        QByteArray tmpBuf(64, 0);
        int rt = 0;
        while(tryTimes < 2) {
            tmpBuf.fill(0);
            rt = libusb_interrupt_transfer(deviceHandle,  endpoint | LIBUSB_ENDPOINT_IN,
                                      (unsigned char *) tmpBuf.data(), INTER_BUF_SIZE, NULL, 2000);
            if(rt == LIBUSB_SUCCESS) {
                rdBuffer.append(tmpBuf);
                if(rdBuffer.size() > 512) {
                    emit finished(IOType::READ, rdBuffer);
                }
            } else {
                qDebug() << "ERROR:" << libusb_error_name(rt);
                tryTimes++;
            }
        }
        emit finished(IOType::READ, rdBuffer);
    }
}

// 异步IO
void LIBUSB_CALL asyncTransferCallback(struct libusb_transfer *transfer) {
    auto type = transfer->type;
    bool success = false;
    switch(transfer->status) {
        case LIBUSB_TRANSFER_COMPLETED: success = true; break;
        case LIBUSB_TRANSFER_ERROR: qWarning() << "异步传输错误,传输类型：" << type; break;
        case LIBUSB_TRANSFER_TIMED_OUT: qWarning() << "异步传输超时，传输类型：" << type; break;
        case LIBUSB_TRANSFER_CANCELLED: qInfo() << "异步传输取消，传输类型：" << type; break;
        case LIBUSB_TRANSFER_STALL: {
            if(type == LIBUSB_TRANSFER_TYPE_BULK || type == LIBUSB_TRANSFER_TYPE_INTERRUPT) {
                qWarning() << "异步传输错误：端点挂起！传输类型：" << type;
            } else if(type == LIBUSB_TRANSFER_TYPE_CONTROL) {
                qWarning() << "异步传输错误：设备不支持控制命令！传输类型：" << type;
            } else {
                qWarning() << "异步传输错误：未知！传输类型：" << type;
            }
        } break;
        case LIBUSB_TRANSFER_NO_DEVICE: qWarning() << "异步传输错误：设备断开连接！"; break;
        case LIBUSB_TRANSFER_OVERFLOW: qWarning() << "异步传输错误：主机收到数据量超过请求数据量！ 传输类型：" << type; break;
        default: qWarning() << " 未知状态！"; break;
    }
    if(success) {
        auto userData = reinterpret_cast<AsyncUserData*>(transfer->user_data);
        QByteArray data((const char*)libusb_control_transfer_get_data(transfer), userData->length);
    } else {

    }
    // 等时传输处理 特殊处理：检查每一个包的status，不能只检查status
    // 获取数据 libusb_get_iso_packet_buffer_simple() 前提是每一个包的大小相同
//    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
//        std::cout << "Transfer completed successfully, " << transfer->actual_length << " bytes received." << std::endl;
//
//        // 处理接收到的数据（根据 your_device_specific_format 解析数据）
//        for (int i = 0; i < transfer->actual_length; ++i) {
//            std::cout << std::hex << static_cast<int>(transfer->buffer[i]) << " ";
//        }
//        std::cout << std::dec << std::endl;
//
//        // 继续提交传输以接收更多数据
//        libusb_submit_transfer(transfer);
//    } else {
//        std::cerr << "Transfer failed with status: " << libusb_error_name(transfer->status) << std::endl;
//        libusb_free_transfer(transfer);
//    }
}

AsyncIO::~AsyncIO() {
    if(transfer != nullptr) {
        libusb_free_transfer(transfer);
    }
}
AsyncIO::AsyncIO(libusb_device_handle *dev_handle, unsigned int timeout,  libusb_transfer_cb_fn cb, QObject *parent)
    : deviceHandle(dev_handle)
    , callback(cb)
    , timeOut(timeout)
    , AbstractIO(parent)
{
}

CtlIOAsync::CtlIOAsync(libusb_device_handle *dev_handle, unsigned int timeout, libusb_transfer_cb_fn cb,  QObject *parent)
    : AsyncIO(dev_handle, timeout, cb, parent)
{
}
void CtlIOAsync::doTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data) {
    if(transfer == nullptr) {
        transfer = libusb_alloc_transfer(0);
    }
    if(ioType == IOType::WRITE) {
        AsyncIO::userData.ioType = ioType;
        AsyncIO::userData.length = data.size();
        buffer.resize(data.size() + LIBUSB_CONTROL_SETUP_SIZE);
        libusb_fill_control_setup((unsigned char*)buffer.data(), bmRequestType, bRequest, wValue, wIndex, wLength);
        buffer.replace(LIBUSB_CONTROL_SETUP_SIZE, data.size(), data);
        libusb_fill_control_transfer(transfer, deviceHandle, (unsigned char*)buffer.data(), callback, (void*)&userData, timeOut);
        auto r = libusb_submit_transfer(transfer);
        if(r != 0) {
            qWarning() << "异步控制提交失败： " << libusb_error_name(r);
        }
    }
}
InterruptIOAsync::InterruptIOAsync(libusb_device_handle *dev_handle, unsigned int timeout, libusb_transfer_cb_fn cb, QObject *parent)
    : AsyncIO(dev_handle, timeout, cb, parent) {
}
void InterruptIOAsync::doTransfer(uint8_t endPoint, IOType ioType, const QByteArray &data) {
//    if(transfer == nullptr) {
//        transfer = libusb_alloc_transfer(0);
//    }
//    AsyncIO::userData.ioType = ioType;
//    if(ioType == IOType::WRITE) {
//        buffer = data;
//        AsyncIO::userData.length = buffer.size();
//        libusb_fill_interrupt_transfer(transfer, deviceHandle, endPoint,  (unsigned char*)buffer.data(), BUFFER_SIZE,
//                                       callback, (void*)&userData, timeOut);
//    } else if(ioType == IOType::READ) {
//        libusb_fill_interrupt_transfer(transfer, deviceHandle, endPoint,  (unsigned char*)buffer.data(), BUFFER_SIZE,
//                                       callback, (void*)&userData, timeOut);
//    }
//    auto r = libusb_submit_transfer(transfer);
//    if(r < 0) {
//        qWarning() << "异步中断传输提交失败：" << libusb_error_name(r);
//    }
}
IsoIOAsync::IsoIOAsync(libusb_device_handle *dev_handle, unsigned int timeout, libusb_transfer_cb_fn cb, QObject *parent)
    : AsyncIO(dev_handle, timeout, cb, parent) {
}
void IsoIOAsync::doTransfer(uint8_t endPoint, IOType ioType, uint8_t pointNum, const QByteArray &data) {
    // 包的长度限制通常为4k
    AsyncIO::userData.ioType = ioType;
    transfer = libusb_alloc_transfer(ISO_PACKET_COUNT);
    for(int i = 0; i < ISO_PACKET_COUNT; i++) {
        transfer->iso_packet_desc[i].length = ISO_SINGLE_PACKET_SIZE;
        transfer->iso_packet_desc[i].actual_length = 0;
    }
    if(ioType == IOType::WRITE) {
        buffer = data;
        AsyncIO::userData.length = buffer.size();
        libusb_fill_iso_transfer(transfer, deviceHandle, LIBUSB_ENDPOINT_OUT | pointNum,  (unsigned char*)buffer.data(), ISO_SINGLE_PACKET_SIZE * ISO_PACKET_COUNT,
                                       ISO_PACKET_COUNT, callback, (void*)&userData, timeOut);
    } else if(ioType == IOType::READ) {
        libusb_fill_iso_transfer(transfer, deviceHandle, LIBUSB_ENDPOINT_IN | pointNum,  (unsigned char*)buffer.data(), ISO_SINGLE_PACKET_SIZE * ISO_PACKET_COUNT,
                                 ISO_PACKET_COUNT, callback, (void*)&userData, timeOut);
    }
    auto r = libusb_submit_transfer(transfer);
    if(r < 0) {
        qWarning() << "异步等时传输提交失败：" << libusb_error_name(r);
    }
}
