#include "iomanager.h"
#include "usbdevice.h"

#include <qdebug.h>

UsbEvtHandler::UsbEvtHandler(QObject *parent) : QObject(parent) {
}
void UsbEvtHandler::onStartHandler() {
    if(startEvtHandle) {
        return;
    } else {
        startEvtHandle = true;
        while(!QThread::currentThread()->isInterruptionRequested() && startEvtHandle) {
            checkHandleRes(libusb_handle_events(nullptr));
        }
    }
}

void UsbEvtHandler::onStopHandler() {
    startEvtHandle = false;
}
void UsbEvtHandler::checkHandleRes(int res) {
    if(res != 0) {
        auto err = libusb_error_name(res);
        qWarning() << "USB事件处理错误：" << err;
        emit errorOccurred(err);
    }
}

IoManager::IoManager(QObject *parent)
    : QObject(parent) {
}

IoManager::~IoManager() {
    for(auto key : transfers.keys()) {
        removeDevice(key);
    }
    if(evtHandlerThr.isRunning()) {
        evtHandlerThr.requestInterruption();
        evtHandlerThr.quit();
        evtHandlerThr.wait();
    }
}

IoManager &IoManager::instance() {
    static IoManager manager;
    return manager;
}
void IoManager::insertTransfer(uint8_t point, IoManager::ThrType thrType, IOType type, UsbDevice* usbDevice) {
    if(!transfers.contains(usbDevice)) {
        transfers[usbDevice] = QMap<QPair<uint8_t, ThrType>, IoInfo>{};
    }
    auto& mp = transfers[usbDevice];
    auto pair = qMakePair(point, thrType);
    if(!mp.contains(pair)) {
        auto ioInfo =  createIoInfo(thrType, usbDevice->getDeviceHandle(), type);
        connectSignals(ioInfo.thr, ioInfo.abstractIo, thrType, usbDevice);
        mp[pair] = ioInfo;
        if(ioInfo.thr != nullptr) {
            ioInfo.abstractIo->moveToThread(ioInfo.thr);
            ioInfo.thr->start();
        }
    }
}
IoManager::IoInfo IoManager::createIoInfo(ThrType thrType, libusb_device_handle* deviceHandle, IOType type) {
    IoInfo ioInfo;
    if(thrType == SYNC_BULK_IO || thrType == SYNC_CTL_IO || thrType == SYNC_INTER_IO || thrType == ASYNC_ISO_IO) {
        ioInfo.thr = new QThread;
    }
    switch((int)thrType) {
        case ThrType::SYNC_CTL_IO: {
            ioInfo.abstractIo = new CtlIOSync(deviceHandle);
        } break;
        case ThrType::SYNC_INTER_IO: {
            ioInfo.abstractIo = new InterruptIOSync(deviceHandle);
        } break;
        case ThrType::SYNC_BULK_IO: {
            ioInfo.abstractIo = new BulkIOSync(type, deviceHandle);
        } break;
        case ThrType::ASYNC_CTL_IO: {
            ioInfo.abstractIo = new CtlIOAsync(deviceHandle);
        } break;
        case ThrType::ASYNC_INTER_IO: {
            ioInfo.abstractIo = new InterruptIOAsync(deviceHandle);
        } break;
        case ThrType::ASYNC_ISO_IO: {
            ioInfo.abstractIo = new IsoIOAsync(deviceHandle);
        } break;
    }
    return ioInfo;
}
void IoManager::connectSignals(QThread *thr, AbstractIO *abstractIo, ThrType thrType, UsbDevice* usbDevice) {
    connect(abstractIo, &AbstractIO::errorOccured, usbDevice, &UsbDevice::onErrorOccurred, Qt::QueuedConnection);
    connect(abstractIo, &AbstractIO::finished, usbDevice, &UsbDevice::onTransferFinished, Qt::QueuedConnection);
    connect(abstractIo, &AbstractIO::timeout, usbDevice, &UsbDevice::onTimeOut, Qt::QueuedConnection);

    if(thr != nullptr) {
        connect(thr, &QThread::finished, abstractIo, &QObject::deleteLater);
    }

    switch((int)thrType) {
        case ThrType::SYNC_CTL_IO: {
            auto ioObj = dynamic_cast<CtlIOSync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doSyncCtlIO, ioObj, &CtlIOSync::doTransfer, Qt::QueuedConnection);
        } break;
        case ThrType::SYNC_INTER_IO: {
            auto ioObj = dynamic_cast<InterruptIOSync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doSyncInterIo, ioObj, &InterruptIOSync::doTransfer, Qt::QueuedConnection);
        } break;
        case ThrType::SYNC_BULK_IO: {
            auto ioObj = dynamic_cast<BulkIOSync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doSyncBulkIO, ioObj, &BulkIOSync::doTransfer, Qt::QueuedConnection);
        } break;
        case ThrType::ASYNC_CTL_IO: {
            auto ioObj = dynamic_cast<CtlIOAsync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doAsyncCtlIO, ioObj, &CtlIOAsync::doTransfer, Qt::QueuedConnection);
        } break;
        case ThrType::ASYNC_INTER_IO: {
            auto ioObj = dynamic_cast<InterruptIOAsync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doAsyncInterIO, ioObj, &InterruptIOAsync::doTransfer);
            connect(usbDevice, &UsbDevice::doAsyncInterIO, this, [&](uint8_t endPoint, IOType ioType, const QByteArray& data) {
                qDebug() << "开始异步传输！";
            });
        } break;
        case ThrType::ASYNC_ISO_IO: {
            auto ioObj = dynamic_cast<IsoIOAsync*>(abstractIo);
            connect(usbDevice, &UsbDevice::doAsyncIsoIO, ioObj, &IsoIOAsync::doTransfer);
        } break;
    }
}

void IoManager::startEvtHandle() {
    if(!evtHandlerThr.isRunning()) {
        auto evtHandler = new UsbEvtHandler;
        evtHandler->moveToThread(&evtHandlerThr);
        connect(&evtHandlerThr, &QThread::finished, evtHandler, &QObject::deleteLater);
        evtHandlerThr.start();
    }
}

void IoManager::removeDevice(UsbDevice *device) {
    if(transfers.size() <= 0) {
        return;
    }
    if(transfers.contains(device)) {
        auto& pointTrans = transfers[device];
        for(auto& ioInfo : pointTrans.values()) {
            if(ioInfo.thr != nullptr && ioInfo.thr->isRunning()) {
                ioInfo.thr->requestInterruption();
                ioInfo.thr->quit();
                ioInfo.thr->wait();
                ioInfo.thr->deleteLater();
            } else {
                ioInfo.abstractIo->deleteLater();
            }
        }
        transfers.remove(device);
    }
}
