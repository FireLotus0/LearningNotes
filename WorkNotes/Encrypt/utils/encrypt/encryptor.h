#pragma once

#include "qtaes/qaesencryption.h"
#include <string>

namespace Encryptor {
// 使用默认秘钥解密解密，用于与设备通信
std::string encrypt(const std::string &data);

std::string decrypt(const std::string &data);

// 使用指定秘钥解密解密，用于与服务器通信
std::string encrypt(const std::string& key, const std::string &data);

std::string decrypt(const std::string& key, const std::string &data);

std::string encrypt(unsigned long sk, const std::string &data);

std::string decrypt(unsigned long sk, const std::string &data);
}