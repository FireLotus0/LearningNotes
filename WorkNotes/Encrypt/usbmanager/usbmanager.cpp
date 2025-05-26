
#include "usbmanager.h"
#include "utils/logger/logger.h"
#include <chrono>

UsbManager::UsbManager() {
    targetId = {0x4831, 0x4831};
    checkError(libusb_init(nullptr) != LIBUSB_SUCCESS);
    listenThr = std::thread(&UsbManager::listenHotPlug, this);
}

UsbManager::~UsbManager() {
    isListenStop.store(true, std::memory_order_release);
    if(listenThr.joinable()) {
        listenThr.join();
    }
    libusb_exit(nullptr);
}

bool UsbManager::checkError(int code) {
    if (code != LIBUSB_SUCCESS) {
        LOG_ERROR(libusb_error_name(code));
        return false;
    }
    return true;
}

void UsbManager::listenHotPlug() {
    libusb_device **devLists;
    std::size_t devCount;
    libusb_device_descriptor devDesc{};
    libusb_device *tmpDevice;
    bool targetDeviceExists{false}, findTarget{false};
    while (!isListenStop.load(std::memory_order_acquire)) {
        devCount = libusb_get_device_list(nullptr, &devLists);
        findTarget = false;
        for (int i = 0; i < devCount; i++) {
            if (checkError(libusb_get_device_descriptor(devLists[i], &devDesc))) {
                if (devDesc.idProduct == targetId.first && devDesc.idVendor == targetId.second) {
                    findTarget = true;
                    tmpDevice = devLists[i];
                    break;
                }
            }
        }
        libusb_free_device_list(devLists, 1);
        if (findTarget && !targetDeviceExists) {
            // 设备插入
            targetDeviceExists = onDevArrived(tmpDevice);
        } else if (!findTarget && targetDeviceExists) {
            // 设备移除
            onDevLeft();
            targetDeviceExists = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

bool UsbManager::onDevArrived(libusb_device *device) {
    std::lock_guard<std::mutex> lk(mt);
    targetDevice = device;
    targetHandle = libusb_open_device_with_vid_pid(nullptr, targetId.second, targetId.first);
    if (targetHandle != nullptr) {
#ifdef OS_UNIX
        checkError(libusb_set_auto_detach_kernel_driver(targetHandle, 1));
        checkError(libusb_set_configuration(targetHandle, 1));
        checkError(libusb_claim_interface(targetHandle, 0));
#endif
        /// TODO: somtimes it worked even busy error reported
        isUsbAvailable = true;
        return true;
    }
    return false;
}

void UsbManager::onDevLeft() {
    std::lock_guard<std::mutex> lk(mt);
    isUsbAvailable = false;
    targetDevice = nullptr;
#ifdef OS_UNIX
    libusb_release_interface(targetHandle, 0);
#endif
    libusb_close(targetHandle);
    targetHandle = nullptr;
}


