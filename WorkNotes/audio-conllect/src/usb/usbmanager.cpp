#include "usbmanager.h"
#include <iostream>
#include <thread>
#include <qtimer.h>

UsbManager &UsbManager::instance() {
    static UsbManager usbManager;
    return usbManager;
}

UsbManager::UsbManager(QObject* parent)
    : QObject(parent)
{
    auto initRes = libusb_init(nullptr);
    if (initRes != 0) {
        std::cout << "Lib Init Failed!" << std::endl;
    } else {
        auto listener = new UsbListener;
        listener->moveToThread(&listenThread);
        connect(&listenThread, &QThread::finished, listener, &QObject::deleteLater);
        connect(this, &UsbManager::startSearch, listener, &UsbListener::listen);
        connect(listener, &UsbListener::deviceChanged, this, &UsbManager::onDeviceChanged, Qt::QueuedConnection);
        listenThread.start();
    }
}

UsbManager::~UsbManager() {
    listenThread.requestInterruption();
    listenThread.quit();
    listenThread.wait();
    for (auto dev: usbDevices) {
        delete dev.second;
    }
    libusb_exit(nullptr);
}

void UsbManager::onDeviceChanged(const ListenData &devs) {
    for(const auto& dev : devs.data.keys()) {
        auto libusbDevPtr = devs.data[dev];
        if(usbDevices.contains(dev)) {
            delete usbDevices[dev].second;;
            usbDevices.remove(dev);
#ifdef DEBUG_TEST
            qInfo() << QString("USB设备拔除! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
            emit connectionChanged(dev.first, dev.second, false);
#else
            if(interestDev.contains(dev.first)) {
                qInfo() << QString("USB Device Removed! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
                emit connectionChanged(dev.first, dev.second, false);
            }
#endif
        } else {
#ifdef DEBUG_TEST
            qInfo() << QString("USB设备插入! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
            usbDevices[dev] = qMakePair(libusbDevPtr, new UsbDevice(libusbDevPtr, nullptr, dev.first, dev.second));
            emit connectionChanged(dev.first, dev.second, usbDevices[dev].second->openDevice() == 0);
#else
            if(interestDev.contains(dev.first)) {
                qInfo() << QString("USB Device Attached! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
                auto usbDevice = new UsbDevice(libusbDevPtr, nullptr, dev.first, dev.second);
                usbDevices[dev] = qMakePair(libusbDevPtr, usbDevice);
                connect(usbDevice, &UsbDevice::information, this, &UsbManager::information);
                connect(usbDevice, &UsbDevice::readFinished, this, &UsbManager::readFinished);

                usbDevice->openDevice();
                emit connectionChanged(dev.first, dev.second, usbDevices[dev].second->isOpened());

            }
#endif
        }
    }
}

void UsbManager::appendInterestDev(const QList<QPair<uint16_t, uint16_t>> &dev) {
    for(const auto& pair : dev) {
        interestDev[pair.first] = pair.second;
    }
}

UsbDevice *UsbManager::getDevice(uint16_t pid, uint16_t vid) {
    auto pair = qMakePair(pid, vid);
    if(usbDevices.contains(pair)) {
        return usbDevices[pair].second;
    }
    return nullptr;
}
