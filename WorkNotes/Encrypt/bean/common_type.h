#pragma once
#include <string>
#include "utils/json/json.h"


enum ErrorCode {
    OK = 0,
    LIBUSB_UNAVAILABLE = 1, // libusb初始化失败
    DEVICE_REMOVED = 2, // 设备移除
    DEVICE_NOT_OPEN,   // 设备未连接
    WRITE_TIME_OUT,
    READ_TIME_OUT,
    LIBUSB_INTERNAL_ERROR,
    KEY_EMPTY_FROM_DOG,
    SERVER_PUB_KEY_INVALID,
    SEND_SESSION_KEY_RESPONSE_FAIL,
};

struct ReadKey {
    enum {
        Type = 1
    };

    std::string cpuId;
    std::string mac;
};

struct ReadKeyFd {
    enum {
        Type = 4
    };

    std::string key;
};

struct GenKey {
    enum {
        Type = 2
    };

    std::string cpuId;
    std::string mac;
    std::string sk;
};

struct GenKeyFd {
    enum {
        Type = 5
    };

    std::string key;
};

struct ReadId {
    enum {
        Type = 3
    };
    uint8_t placeHolder = 0;
};

struct ReadIdFd {
    enum {
        Type = 6
    };
    std::string id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReadKey, cpuId, mac)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReadKeyFd, key)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GenKey, cpuId, mac, sk)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GenKeyFd, key)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReadId, placeHolder)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReadIdFd, id)
