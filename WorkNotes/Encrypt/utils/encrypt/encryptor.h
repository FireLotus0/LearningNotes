#pragma once

#include "qtaes/qaesencryption.h"
#include <string>
#include <QRandomGenerator64>

namespace Encryptor {
// 使用默认秘钥解密解密，用于与设备通信
std::string encrypt(const std::string &data);

std::string decrypt(const std::string &data);

// 使用指定秘钥解密解密，用于与服务器通信
std::string encrypt(const std::string& key, const std::string &data);

std::string decrypt(const std::string& key, const std::string &data);

std::string encrypt(unsigned long sk, const std::string &data);

std::string decrypt(unsigned long sk, const std::string &data);

std::string sha256(const std::string& data);

std::string toBase64(const std::string& data);

std::string fromBase64(const std::string& data);

class DHCodec {
public:
    static quint64 createPrivateKey() {
        return g + QRandomGenerator64::global()->generate() % (p - g + 1);
    }

    static quint64 getPublicKey(quint64 privateKey) {
        return modPow(g, privateKey, p);
    }

    static quint64 getSharedKey(quint64 publicKey, quint64 privateKey) {
        return modPow(publicKey, privateKey, p);
    }

private:
    static quint64 modPow(quint64 x, quint64 y, quint64 n) {
        quint64 result = 1;
        x %= n;

        while (y > 0) {
            if (y & 1) {
                result = (result * x) % n;
            }
            x = (x * x) % n;
            y >>= 1;
        }
        return result;
    }

private:
    inline static unsigned long p = 1371857489UL;
    inline static unsigned long g = 451517198UL;
};

}