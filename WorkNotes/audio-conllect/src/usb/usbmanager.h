#pragma once

#include "usbdevice.h"
#include "usblistener.h"
#include <qdebug.h>
#include <qmap.h>
#include <qstring.h>
#include <qthread.h>
#include <qtimer.h>

//struct UsbId {
//    uint
//};

class UsbManager : public QObject{
    Q_OBJECT
public:
    static UsbManager &instance();

    void appendInterestDev(const QList<QPair<uint16_t, uint16_t >>& dev);

    UsbDevice* getDevice(uint16_t pid, uint16_t vid);
signals:
    void startSearch();

    void connectionChanged(uint16_t pid, uint16_t vid, bool connected);

    void information(const QString& errStr, bool isError);

    void readFinished(const QByteArray&);

public slots:
    void onDeviceChanged(const ListenData& devices );

public:
     QMap<QPair<uint16_t , uint16_t >,QPair<libusb_device*, UsbDevice*>> usbDevices{};
     QMap<uint16_t, uint16_t > interestDev;

private:
    UsbManager(QObject* parent = nullptr);

    ~UsbManager();

private:
    QVector<QThread> workers;
    QThread listenThread;
};




