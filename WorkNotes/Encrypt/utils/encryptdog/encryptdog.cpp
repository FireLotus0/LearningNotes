#include "encryptdog.h"
#include "utils/hostinfo/hostinfo.h"

EncryptDog::EncryptDog() {
//    codec.registerCallback<ReadKeyFd::Type>(std::bind(&EncryptDog::onReadKeyFdDecodeFinish, this, std::placeholders::_1));
//    codec.registerCallback<GenKeyFd::Type>(std::bind(&EncryptDog::onGenKeyFdDecodeFinish, this, std::placeholders::_1));
//    codec.registerCallback<ReadIdFd::Type>(std::bind(&EncryptDog::onReadIdFdDecodeFinish, this, std::placeholders::_1));
    usbManager = UsbManager::instance();
    usbManager->registerCallback([&]{
        auto readIdTest = readId();
        LOG_INFO("Read Id Test:", readIdTest.first, readIdTest.second);
        readIdTest = refreshKey("101111");
        LOG_INFO("Refresh Key Test:", readIdTest.first, readIdTest.second);
        readIdTest = readKey();
        LOG_INFO("Read Key Test:", readIdTest.first, readIdTest.second);
    }, nullptr);
}

EncryptDog::~EncryptDog() {
    delete usbManager;
}

std::pair<int, std::string> EncryptDog::readId() {
    // 写入读取命令，读取反馈
    auto rt = usbManager->write(encode(ReadId{}));
    if(rt == ErrorCode::OK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto res = usbManager->read();
        if(res.first == ErrorCode::OK) {
            auto decodeFrame = codec.decode<ReadIdFd>(res.second);
            if(decodeFrame.first) {
                return {ErrorCode::OK, decodeFrame.second.id};
            }
        }
    }
    return {rt, {}};
}

std::pair<int, std::string> EncryptDog::readKey() {
    ReadKey data;
    data.cpuId = HostInfo::getCpuId();
    data.mac = HostInfo::getMac();
    auto rt = usbManager->write(encode(data));
    if(rt == ErrorCode::OK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto res = usbManager->read();
        if(res.first == ErrorCode::OK) {
            auto decodeFrame = codec.decode<ReadKeyFd>(res.second);
            if(decodeFrame.first) {
                return {ErrorCode::OK, decodeFrame.second.key};
            }
        }
    }
    return {rt, {}};
}

std::pair<int, std::string> EncryptDog::refreshKey(const std::string &sk) {
    GenKey data;
    data.cpuId = HostInfo::getCpuId();
    data.mac = HostInfo::getMac();
    data.sk = sk;
    auto rt = usbManager->write(encode(data));
    if(rt == ErrorCode::OK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto res = usbManager->read();
        if(res.first == ErrorCode::OK) {
            auto decodeFrame = codec.decode<GenKeyFd>(res.second);
            if(decodeFrame.first) {
                return {ErrorCode::OK, decodeFrame.second.key};
            }
        }
    }
    return {rt, {}};
}

//void EncryptDog::onReadKeyFdDecodeFinish(const ByteArray &data) {
//    /// TODO: AES 解密，json反序列化
//    try
//    {
//        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
//        LOG_INFO("ReadKeyFd Parse Success:", j["key"]);
//    }
//    catch(std::exception& e) // 捕获异常
//    {
//        LOG_ERROR("ReadKeyFd Parse Failed:", e.what());
//    }
//
//}
//
//void EncryptDog::onGenKeyFdDecodeFinish(const ByteArray &data) {
//    try
//    {
//        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
//        LOG_INFO("GenKeyFd Parse Success:", j["key"]);
//    }
//    catch(std::exception& e) // 捕获异常
//    {
//        LOG_ERROR("GenKeyFd Parse Failed:", e.what());
//    }
//
//}
//
//void EncryptDog::onReadIdFdDecodeFinish(const ByteArray &data) {
//    try
//    {
//        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
//        LOG_INFO("ReadIdFd Parse Success:", j["id"]);
//    }
//    catch(std::exception& e) // 捕获异常
//    {
//        LOG_ERROR("ReadIdFd Parse Failed:", e.what());
//    }
//
//}
