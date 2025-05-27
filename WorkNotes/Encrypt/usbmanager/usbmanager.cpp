
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
        if (findTarget && !targetDeviceExists) {
            // 设备插入
            targetDeviceExists = onDevArrived(tmpDevice);
        } else if (!findTarget && targetDeviceExists) {
            // 设备移除
            onDevLeft();
            targetDeviceExists = false;
        }
        libusb_free_device_list(devLists, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

bool UsbManager::onDevArrived(libusb_device *device) {
    {
        std::lock_guard<std::mutex> lk(mt);
        targetDevice = device;
        targetHandle = libusb_open_device_with_vid_pid(nullptr, targetId.second, targetId.first);
    }

    if (targetHandle != nullptr) {
//#ifdef OS_UNIX
        checkError(libusb_set_auto_detach_kernel_driver(targetHandle, 1));
        checkError(libusb_set_configuration(targetHandle, 1));
        checkError(libusb_claim_interface(targetHandle, 0));
//#endif
        /// TODO: sometimes it worked even busy error reported
        isUsbAvailable = true;
        LOG_INFO("Device Arrive!");
        if(onArriveCb != nullptr) {
            onArriveCb();
        }
        return true;
    }
    return false;
}

void UsbManager::onDevLeft() {
    {
        std::lock_guard<std::mutex> lk(mt);
        isUsbAvailable = false;
        targetDevice = nullptr;
    }

#ifdef OS_UNIX
    libusb_release_interface(targetHandle, 0);
#endif
    LOG_INFO("Device Left!");
    libusb_close(targetHandle);
    targetHandle = nullptr;
    if(onLeftCb != nullptr) {
        onLeftCb();
    }
}

int UsbManager::write(const ByteArray &data) {
    if(!isUsbAvailable) {
        LOG_WARNING("Write Failed: Device Not Available");
        return ErrorCode::DEVICE_NOT_OPEN;
    }
    int totalLength = data.length();
    int transferred = 0, tmpTransferred = 0;
    auto sourceData = data.toString();
    char sendBuf[64];
    int rt = 0;
    while(transferred < totalLength) {
        memset(sendBuf, 0, 64);
        memcpy(sendBuf, sourceData.data() + transferred, (packageSize < (totalLength - transferred) ? packageSize : (totalLength - transferred)));
        {
            std::lock_guard<std::mutex> lk(mt);
            if(targetHandle == nullptr) {
                return ErrorCode::DEVICE_NOT_OPEN;
            }
            rt = libusb_interrupt_transfer(targetHandle, 1 | LIBUSB_ENDPOINT_OUT,
                                                (unsigned char*)sendBuf, 64, &tmpTransferred, 2000);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        if(!checkError(rt)) {
            return rt == LIBUSB_ERROR_TIMEOUT ? ErrorCode::WRITE_TIME_OUT : ErrorCode::LIBUSB_INTERNAL_ERROR;
        } else {
            LOG_INFO("Write Data:", ByteArray(sendBuf, 64).toHexStr());
#ifdef OS_WINDOWS
            transferred += tmpTransferred - 1;
#elif defined(OS_UNIX)
            transferred += tmpTransferred;
#endif
        }
    }
    return ErrorCode::OK;
}

std::pair<int, ByteArray> UsbManager::read() {
    std::pair<int, ByteArray> res;
    int tryTimes = 0, rt = 0;
    char readBuf[64];
    int tmpTransferred;
    while(tryTimes < 3) {
        memset(readBuf, 0, 64);
        {
            std::lock_guard<std::mutex> lk(mt);
            if(targetHandle == nullptr) {
                res.first = ErrorCode::DEVICE_NOT_OPEN;
                return res;
            }
            rt = libusb_interrupt_transfer(targetHandle, 1 | LIBUSB_ENDPOINT_IN, (unsigned char*)readBuf, 64, &tmpTransferred, 2000);
            if(!checkError(rt)) {
                if(rt == LIBUSB_ERROR_TIMEOUT) {
                    tryTimes++;
                } else {
                    res.first = ErrorCode::LIBUSB_INTERNAL_ERROR;
                    return res;
                }
            } else {
                res.second.append(ByteArray(readBuf, 64));
            }
        }
    }
    res.first = ErrorCode::OK;
    return res;
}

void UsbManager::registerCallback(const std::function<void()> &arriveCb, const std::function<void()> &leftCb) {
    onArriveCb = arriveCb;
    onLeftCb = leftCb;
}


