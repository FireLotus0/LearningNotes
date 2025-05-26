#pragma once
#include "libusb.h"
#include "utils/singleinstance/singleinstance.h"
#include "utils/codec/framecodec.h"

#include <thread>
#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <atomic>

enum ErrorCode {
    OK = 0,
    LIBUSB_UNAVAILABLE = 1, // libusb初始化失败
    DEVICE_REMOVED = 2, // 设备移除

};

class UsbManager : public SingleInstance<UsbManager>{
public:
    template<typename T>


private:
    UsbManager();

    ~UsbManager();
private:
    bool checkError(int code);

    void listenHotPlug();

    bool onDevArrived(libusb_device* device);

    void onDevLeft();

private:
    FrameCodec frameCodec;
    std::mutex mt;
    bool isUsbAvailable{false};
    std::thread listenThr;
    std::atomic<bool> isListenStop{false};
    std::pair<uint16_t, uint16_t> targetId;
    libusb_device* targetDevice;
    libusb_device_handle* targetHandle;
    std::size_t packageSize{64};
};

