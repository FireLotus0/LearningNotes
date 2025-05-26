#pragma once

#include "bean/common_type.h"
#include "utils/codec/framecodec.h"
#include "utils/singleinstance/singleinstance.h"
#include "utils/json/json.h"
#include "utils/encrypt/encryptor.h"

using nlohmann::json;
/// JSOO 处理 AES处理
class DataHandler : public SingleInstance<DataHandler>{
public:
    DataHandler();

    template<typename T>
    ByteArray encode(const T& data) {
        if constexpr (!std::is_same_v<T, ReadId>) {
            json obj = data;
            /// TODO: 加密处理
            auto jsonStr = obj.dump();
            LOG_DEBUG("ReadKey Json Dump:", jsonStr);
            // 使用默认秘钥加密
            return codec.encode<T::Type>(ByteArray(Encryptor::encrypt(jsonStr)));
        } else {
            return codec.encode<T::Type>(ByteArray());
        }
    }

    void onReadKeyFdDecodeFinish(const ByteArray& data);
    void onGenKeyFdDecodeFinish(const ByteArray& data);
    void onReadIdFdDecodeFinish(const ByteArray& data);

private:
    FrameCodec codec;
};
