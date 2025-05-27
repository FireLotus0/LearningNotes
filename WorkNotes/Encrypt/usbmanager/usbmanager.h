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
    DEVICE_NOT_OPEN,   // 设备未连接
    WRITE_TIME_OUT,
    READ_TIME_OUT,
    LIBUSB_INTERNAL_ERROR,
};

class UsbManager : public SingleInstance<UsbManager>{
public:
    int write(const ByteArray& data);

    std::pair<int, ByteArray> read();

    UsbManager();

    ~UsbManager();

    void registerCallback(const std::function<void()>& arriveCb, const std::function<void()>& leftCb);

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
    std::function<void()> onArriveCb{nullptr}, onLeftCb{nullptr};
};

