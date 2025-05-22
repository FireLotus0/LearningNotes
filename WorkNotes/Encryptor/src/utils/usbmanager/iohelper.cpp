#include "iohelper.h"
#include "src/utils/logger/logger.h"
#include "src/utils/aes/encry_aes.h"
#include "src/utils/hostinfo/hostinfo.h"
#include <qdebug.h>
#include <qthread.h>

Q_DECLARE_METATYPE(ET::IOType)

namespace ET {
AbstractIO::AbstractIO(QObject *parent) : QObject(parent) {
    qRegisterMetaType<IOType>("IOType");
}

bool AbstractIO::checkResult(int res, IOType ioType) {
    if(res == 0) {
        return true;
    } else if(res == LIBUSB_ERROR_TIMEOUT) {
        emit timeout(ioType);
    }
    else {
        auto str = libusb_error_name(res);
        emit errorOccured(str, res);
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
void CtlIOSync::doTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data, int cmdType) {
}

BulkIOSync::BulkIOSync(IOType type, libusb_device_handle *dev_handle, int buffSz, unsigned int timeout, QObject *parent)
        : SyncIO(dev_handle, buffSz, timeout, parent)
        , ioType(type)
{
}
void BulkIOSync::doTransfer(unsigned char endpoint, int length, IOType type, const QByteArray& data, int cmdType)
{
}
InterruptIOSync::InterruptIOSync(IOType type, libusb_device_handle *dev_handle, int buffSz, unsigned int timeout, QObject *parent)
        : SyncIO(dev_handle, INTER_SYNC_BUF_SIZE, timeout, parent)
        , ioType(type)
{
    qRegisterMetaType<IOType>();
}
void InterruptIOSync::doTransfer(unsigned char endpoint,  IOType type, int cmdType, unsigned long sk) {
    if(ioType != type) {
        return;
    }
    // 1 Read Key 2 Refresh Key 3 Read Id
    Q_ASSERT(cmdType < 4);
    BlockIo blockIo(cmdType, deviceHandle, sk, endpoint);
    auto res = blockIo.doBlockIo();
    if(!res.isEmpty()) {
        emit finished(type, res.toLatin1(), cmdType);
    } else {
        emit errorOccured("USB Transfer Error: ", type == WRITE ? (int)ErrCode::USB_WRITE_ERROR : (int)ErrCode::USB_READ_ERROR);
    }
}

// 异步IO
void LIBUSB_CALL asyncTransferCallback(struct libusb_transfer *transfer) {
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
}
InterruptIOAsync::InterruptIOAsync(libusb_device_handle *dev_handle, unsigned int timeout, libusb_transfer_cb_fn cb, QObject *parent)
        : AsyncIO(dev_handle, timeout, cb, parent) {
}
void InterruptIOAsync::doTransfer(uint8_t endPoint, IOType ioType, const QByteArray &data) {
}
IsoIOAsync::IsoIOAsync(libusb_device_handle *dev_handle, unsigned int timeout, libusb_transfer_cb_fn cb, QObject *parent)
        : AsyncIO(dev_handle, timeout, cb, parent) {
}
void IsoIOAsync::doTransfer(uint8_t endPoint, IOType ioType, uint8_t pointNum, const QByteArray &data) {
}

BlockIo::BlockIo(int type, libusb_device_handle *device, unsigned long sk, uint8_t point)
    : ioType(type)
    , deviceHandle(device)
    , sk(sk)
    , endPoint(point)
{

}

QString BlockIo::doBlockIo() {
    protocol_codec::ProtocolCodecEngine tmpCodecEngine;
    tmpCodecEngine.frameDeclare("H(AAFF)S1CV(CRC16)E(AF)");
    tmpCodecEngine.setVerifyFlags("HSC");

    QByteArray buf, codeData;
    buf.fill(0, 64);

    // 1 Read Key 2 Refresh Key 3 Read Id
    switch(ioType) {
        case 1:  {
            tmpCodecEngine.registerType<ReadKey, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<ReadKeyFd>>(this, &BlockIo::onGetKeyFinished);
            ReadKey data;
            data.cpuId = HostInfo::getCpuId();
            data.mac = HostInfo::getMac();
            codeData = tmpCodecEngine.encode<ReadKey>(data);
            break;
        }
        case 2: {
            tmpCodecEngine.registerType<GenKey, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<GenKeyFd>>(this, &BlockIo::onRefreshKeyFinished);
            GenKey data;
            data.cpuId = HostInfo::getCpuId();
            data.mac = HostInfo::getMac();
            data.sk = QString::number(sk);
            codeData = tmpCodecEngine.encode<GenKey>(data);
            break;
        }
        case 3: {
            tmpCodecEngine.registerType<ReadId, EncryptCodec>();
            tmpCodecEngine.registerType<EncryptCodec<ReadIdFd>>(this, &BlockIo::onReadIdFinished);
            ReadId data;
            codeData = tmpCodecEngine.encode<ReadId>(data);
            break;
        }
    }

    int dataSize = codeData.size(), transferred = 0, tmpTransferred = 0;
    while(transferred < dataSize) {
        buf.fill(0);
        memcpy(buf.data(), codeData.data() + transferred, qMin((dataSize - transferred), 64));
        auto transCode = libusb_interrupt_transfer(deviceHandle, endPoint | LIBUSB_ENDPOINT_OUT,
                                                   (unsigned char*)buf.data(),  INTER_SYNC_BUF_SIZE, &tmpTransferred, 2000);
        if(transCode != 0) {
            auto str = libusb_error_name(transCode);
            LOG_ERROR("Transfer Error:", str);
            return "";
        } else {
            transferred += (tmpTransferred - 1);
        }
        QThread::msleep(3);
    }
    while(result.isEmpty()) {
        buf.fill(' ');
        auto transCode = libusb_interrupt_transfer(deviceHandle, endPoint | LIBUSB_ENDPOINT_IN, (unsigned char*)buf.data(), 64, NULL, 5000);
        if(transCode != LIBUSB_SUCCESS) {
            auto str = libusb_error_name(transCode);
            LOG_ERROR("Transfer Error:", str);
            return "";
        } else {
            tmpCodecEngine.appendBuffer(QByteArray::fromRawData((const char*)buf, 64));
        }
        QThread::msleep(3);
    }
    return result;
}


}
