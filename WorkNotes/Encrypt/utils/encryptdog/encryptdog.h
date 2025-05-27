#pragma once

#include "bean/common_type.h"
#include "utils/codec/framecodec.h"
#include "utils/singleinstance/singleinstance.h"
#include "utils/json/json.h"
#include "utils/encrypt/encryptor.h"
#include "usbmanager/usbmanager.h"

using nlohmann::json;

/// JSOO 处理 AES处理
class EncryptDog : public SingleInstance<EncryptDog> {
public:
    EncryptDog();

    ~EncryptDog();

    template<typename T>
    ByteArray encode(const T &data) {
        json obj = data;
        auto jsonStr = obj.dump();
        LOG_DEBUG("ReadKey Json Dump:", jsonStr);
        // 使用默认秘钥加密
        return codec.encode<T::Type>(ByteArray(Encryptor::encrypt(jsonStr)));
    }

    template<typename T>
    static ByteArray getFrame(const T &data) {
        FrameCodec codec;
        json obj = data;
        auto jsonStr = obj.dump();
        LOG_DEBUG("ReadKey Json Dump:", jsonStr);
        // 使用默认秘钥加密
        return codec.encode<T::Type>(ByteArray(Encryptor::encrypt(jsonStr)));
    }

    std::pair<int, std::string> readId();

    std::pair<int, std::string> readKey();

    std::pair<int, std::string> refreshKey(const std::string &sk);

//    void onReadKeyFdDecodeFinish(const ByteArray& data);
//    void onGenKeyFdDecodeFinish(const ByteArray& data);
//    void onReadIdFdDecodeFinish(const ByteArray& data);

private:
    FrameCodec codec;
    UsbManager *usbManager;
};
