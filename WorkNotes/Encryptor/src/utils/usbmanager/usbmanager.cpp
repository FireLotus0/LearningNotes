#include "usbmanager.h"
#include "usbinfo.h"
#include "src/utils/logger/logger.h"
#include <iostream>
#include <thread>
#include <qtimer.h>

namespace ET {

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
            if(interestDev.contains(dev.first)) {
                qInfo() << QString("USB Device Removed! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
                emit connectionChanged(dev.first, dev.second, false);
            }
        } else {
            if(interestDev.contains(dev.first)) {
                qInfo() << QString("USB Device Attached! PID:%1 VID:%2").arg(dev.first).arg(dev.second);
                auto usbDevice = new UsbDevice(libusbDevPtr, nullptr, dev.first, dev.second);
                usbDevices[dev] = qMakePair(libusbDevPtr, usbDevice);
                connect(usbDevice, &UsbDevice::information, this, &UsbManager::information);
                connect(usbDevice, &UsbDevice::readKeyFinished, this, &UsbManager::readKeyFinished);
                connect(usbDevice, &UsbDevice::readIdFinished, this, &UsbManager::readIdFinished);
                connect(usbDevice, &UsbDevice::generateKeyFinished, this, &UsbManager::generateKeyFinished);

                usbDevice->openDevice();
                auto success = (usbDevices[dev].second->isOpened() ? "Success!" : "Failed!");
                LOG_INFO("Open Usb Device: PID=", dev.first, "VID=", dev.second, " RES:", success );
                emit connectionChanged(dev.first, dev.second, usbDevices[dev].second->isOpened());
            }
        }
    }
}

void UsbManager::appendInterestDev(const QList<QPair<uint16_t, uint16_t>> &dev) {
    if(dev.size() == 1) {
        devId = dev.first();
    }
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

void UsbManager::getKey(UsbDevice* device) {
#ifndef OS_LINUX
    device->getKeyNonBlock();
#endif
}

void UsbManager::getId(UsbDevice* device) {
#ifndef OS_LINUX
    device->getIdNonBlock();
#endif
}

void UsbManager::generateKey(UsbDevice* device, quint64 sk) {
#ifndef OS_LINUX
    device->generateKeyNonBlock(sk);
#endif
}

UsbDevice *UsbManager::getDevice(const QPair<uint16_t, uint16_t> &devId) {
    if(usbDevices.contains(devId)) {
        return usbDevices[devId].second;
    }
    return nullptr;
}

void UsbManager::startSearch() {
    auto listener = new UsbListener;
    listener->moveToThread(&listenThread);
    connect(&listenThread, &QThread::finished, listener, &QObject::deleteLater);
    connect(listener, &UsbListener::deviceChanged, this, &UsbManager::onDeviceChanged, Qt::QueuedConnection);
    listenThread.start();
}

}