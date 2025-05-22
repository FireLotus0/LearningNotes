#pragma once

#include "usbdevice.h"
#include <qobject.h>
#include <qmap.h>

namespace ET {
#define SCAN_FREQ 100
/**
 * @brief Windows平台上监听热插拔事件
 */
struct ListenData {
    QMap<QPair<uint16_t, uint16_t>, libusb_device*> data;
};

class UsbListener : public QObject{
Q_OBJECT
public:
    UsbListener(QObject* parent = nullptr);

public slots:
    void listen();

signals:
    void deviceChanged(const ListenData& devices);

private:
    ListenData devices, tmpDevices;
    libusb_device_descriptor deviceDescriptor;
};

}

Q_DECLARE_METATYPE(ET::ListenData)

