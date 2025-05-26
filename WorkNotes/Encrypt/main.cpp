#include <iostream>
#include <vector>
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "utils/codec/framecodec.h"
#include "utils/json/json.h"
#include "bean/common_type.h"

using json = nlohmann::json;


void onDecodeFinish(const ByteArray& d) {
    LOG_INFO("DECODE: " , d.toHexStr());
    try
    {
        auto j = json::parse(d.toString());  // 从字符串反序列化
        LOG_INFO("JSON parse:", j["cpuId"], j["mac"], j["sk"]);
    }
    catch(std::exception& e) // 捕获异常
    {
        LOG_ERROR("Json Parse Failed:", e.what());
    }
}

int main()
{
    FrameCodec codec;
    codec.registerCallback<2>(onDecodeFinish);
    json j = GenKey{};
    j["cpuId"] = "12th Gen Intel(R) Core(TM) i5-12400";
    j["mac"] = "EC:D6:8A:FE:56:D5";
    j["sk"] = "765478";

    LOG_INFO("Gen Key Json Text:", j.dump());
    auto data = codec.encode<GenKey::Type>(ByteArray(j.dump()));
    codec.appendBuffer(data);


    return 0;
}