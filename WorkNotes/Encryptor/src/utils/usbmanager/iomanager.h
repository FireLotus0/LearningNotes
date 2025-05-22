#pragma once

#include "iohelper.h"
#include <qobject.h>
#include <qthread.h>
#include <qmap.h>

namespace ET{
class UsbDevice;
class UsbEvtHandler : public QObject
{
Q_OBJECT
public:
    UsbEvtHandler(QObject *parent = nullptr);
signals:
    void errorOccurred(const QString& errStr);
public slots:
    void onStartHandler();

    void onStopHandler();
private:
    void checkHandleRes(int res);
private:
    bool startEvtHandle = false;
    int r;
};

class IoManager : public QObject
{
Q_OBJECT
    struct IoInfo{
        QThread* thr = nullptr;
        AbstractIO* abstractIo = nullptr;
    };
public:
    enum ThrType {
        SYNC_CTL_IO,
        SYNC_INTER_IO,
        SYNC_BULK_IO,
        ASYNC_CTL_IO,
        ASYNC_INTER_IO,
        ASYNC_ISO_IO
    };
public:
    static IoManager& instance();

    void startEvtHandle();

    void insertTransfer(uint8_t point, ThrType thrType, IOType type, UsbDevice* usbDevice);

    void removeDevice(UsbDevice* device);
signals:


private:
    explicit IoManager(QObject *parent = nullptr);

    ~IoManager();

    IoInfo createIoInfo(ThrType thrType, libusb_device_handle* deviceHandle, IOType type);

    void connectSignals(QThread* thr, AbstractIO* abstractIo, ThrType thrType, UsbDevice* usbDevice);
private:
    QMap<UsbDevice*, QMap<QPair<uint8_t, ThrType>, IoInfo>> transfers;
    QThread evtHandlerThr;
};

}

