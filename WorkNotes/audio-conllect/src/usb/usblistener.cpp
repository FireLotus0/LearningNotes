#include "usblistener.h"
#include <qthread.h>
#include <qdebug.h>

void UsbListener::listen() {
    libusb_device **devLists;
    int devCount = 0;
    bool isInsert;
    while(!QThread::currentThread()->isInterruptionRequested()) {
        devCount = libusb_get_device_list(nullptr, &devLists);
        qDebug() << "Usb Device Count=" << devCount;
        tmpDevices.data.clear();
        if (devCount <= 0) {
            emit deviceChanged(tmpDevices);
        } else if(devCount != devices.data.size()) {
            isInsert = devCount > devices.data.size();
            if(isInsert) {
                for (int i = 0; i < devCount; i++) {
                    // 获取PID，VID
                    auto ret = libusb_get_device_descriptor(devLists[i], &deviceDescriptor);
                    if(ret >= 0) {
                        auto pairId = qMakePair(deviceDescriptor.idProduct, deviceDescriptor.idVendor);
                        qDebug() << "Device: PID=" << deviceDescriptor.idProduct << " VID=" << deviceDescriptor.idVendor;
                        if(!devices.data.contains(pairId)) {
                            tmpDevices.data[pairId] = devLists[i];
                            devices.data[pairId] = devLists[i];
                        }
                    }
                }
                if(!tmpDevices.data.isEmpty()) {
                    emit deviceChanged(tmpDevices);
                }
            } else {
                tmpDevices.data = devices.data;
                for (int i = 0; i < devCount; i++) {
                    auto ret = libusb_get_device_descriptor(devLists[i], &deviceDescriptor);
                    if(ret >= 0) {
                        tmpDevices.data.remove(qMakePair(deviceDescriptor.idProduct, deviceDescriptor.idVendor));
                    }
                }
                for(const auto& key : tmpDevices.data.keys()) {
                    devices.data.remove(key);
                }
                emit deviceChanged(tmpDevices);
            }
        }
        libusb_free_device_list(devLists, 1);
        QThread::msleep(SCAN_FREQ);
    }
}
UsbListener::UsbListener(QObject *parent) : QObject(parent) {
    qRegisterMetaType<ListenData>();
}