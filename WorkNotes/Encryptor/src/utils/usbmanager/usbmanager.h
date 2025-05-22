#pragma once

#include "usbdevice.h"
#include "usblistener.h"

#include <qdebug.h>
#include <qmap.h>
#include <qstring.h>
#include <qthread.h>
#include <qtimer.h>

namespace ET {

class UsbManager : public QObject{
Q_OBJECT
public:
    static UsbManager &instance();

    void appendInterestDev(const QList<QPair<uint16_t, uint16_t >>& dev);

    UsbDevice* getDevice(uint16_t pid, uint16_t vid);

    UsbDevice* getDevice(const QPair<uint16_t, uint16_t>& devId);

    void getKey(UsbDevice* device);

    void getId(UsbDevice* device);

    void generateKey(UsbDevice* device, quint64 sk);

    void startSearch();

signals:

    void connectionChanged(uint16_t pid, uint16_t vid, bool connected);

    void information(const QString& errStr, int errCode);

    void readKeyFinished(const QString& key);

    void readIdFinished(const QString& id);

    void generateKeyFinished(const QString& key);

public slots:
    void onDeviceChanged(const ListenData& devices );
public:
    inline static QMap<QPair<uint16_t , uint16_t >,QPair<libusb_device*, UsbDevice*>> usbDevices{};
    inline static QMap<uint16_t, uint16_t > interestDev;

private:
    UsbManager(QObject* parent = nullptr);

    ~UsbManager();

private:
    bool isRunning = false;
    QVector<QThread> workers;
    QThread listenThread;
    QPair<uint16_t, uint16_t> devId;
};

}


