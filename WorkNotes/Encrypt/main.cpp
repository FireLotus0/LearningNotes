#include <iostream>
#include <vector>
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "utils/codec/framecodec.h"
#include "utils/json/json.h"
#include "bean/common_type.h"
#include "utils/encryptdog/encryptdog.h"

using json = nlohmann::json;

// 测试可读字符串数据编码成帧并解码
void testReadable() {
    GenKeyFd genKeyFd;
    genKeyFd.key = ByteArray::fromHex("AAFF6202000E25EE0CA4B7883DFFAE800024B9FFA2D46E27E3383CA40688904"
                                      "906765E31E8D07CA5AD351CEF2018E0D12BE0494B2E317BCE748B145C0868E47139"
                                      "DF540BA094BC696BC268287A98941E408B71E30D4F21BD66DE849494114CDF2F2B8EFE7AC9A1AF").toString();
//    genKeyFd.key = QByteArray::fromStdString( genKeyFd.key).toBase64().toStdString();
    LOG_INFO("key:", genKeyFd.key);
    auto frame =  EncryptDog::getFrame<GenKeyFd>(genKeyFd);
    LOG_INFO("receive:", frame.toHexStr());

    FrameCodec codec;
    codec.decode<GenKeyFd>(frame);
}


int main() {
    EncryptDog::instance();


//    FrameCodec codec;
//    auto res = codec.decode<GenKey>(ByteArray::fromHex(
//            "AAFF4205008CCD4B0E57C701B3A08DEEB79EE47DB41F1F4EDCCB030FAE327B30FFD1DD604EC3A20754043BE618F74BED301963E93430719E6CDC22BFFFA2EFEEAD8F437D2C2119AF"));
//    assert(res.first);
//    testReadable();
    while (true) {

    }
    return 0;
}