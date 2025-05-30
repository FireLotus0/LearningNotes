#include <iostream>
#include <vector>
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "utils/codec/framecodec.h"
#include "utils/json/json.h"
#include "bean/common_type.h"
#include "encryptdog/encryptdog.h"
#include "utils/hostinfo/hostinfo.h"
#include "http/httpmanager.h"
#include "enctool/enctool.h"

using json = nlohmann::json;

//// 测试可读字符串数据编码成帧并解码
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

void testHostInfo() {
    LOG_DEBUG("CPU ID=",HostInfo::getCpuId(), "MAC=", HostInfo::getMac());
}

void testGetPubKey() {
    auto http = HttpManager::instance();
    LOG_INFO("Test Get Server Key:", http->getServerPubKey(123455));
}

void testWindowsRefreshKey() {
    char buf[128];
    memset(buf, 0, 128);
    EncTool::instance()->refreshKey(buf);
}

int main() {
//    testHostInfo();
//    EncryptDog::instance();



//    testGetPubKey();

    testWindowsRefreshKey();
    while(true) {

    }
    return 0;
}