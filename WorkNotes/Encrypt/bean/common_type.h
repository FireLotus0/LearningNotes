#pragma once
#include <string>
#include "utils/json/json.h"

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