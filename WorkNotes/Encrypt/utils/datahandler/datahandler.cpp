#include "datahandler.h"

DataHandler::DataHandler() {
    codec.registerCallback<ReadKeyFd::Type>(std::bind(&DataHandler::onReadKeyFdDecodeFinish, this, std::placeholders::_1));
    codec.registerCallback<GenKeyFd::Type>(std::bind(&DataHandler::onGenKeyFdDecodeFinish, this, std::placeholders::_1));
    codec.registerCallback<ReadIdFd::Type>(std::bind(&DataHandler::onReadIdFdDecodeFinish, this, std::placeholders::_1));

}

void DataHandler::onReadKeyFdDecodeFinish(const ByteArray &data) {
    /// TODO: AES 解密，json反序列化
    try
    {
        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
        LOG_INFO("ReadKeyFd Parse Success:", j["key"]);
    }
    catch(std::exception& e) // 捕获异常
    {
        LOG_ERROR("ReadKeyFd Parse Failed:", e.what());
    }

}

void DataHandler::onGenKeyFdDecodeFinish(const ByteArray &data) {
    try
    {
        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
        LOG_INFO("GenKeyFd Parse Success:", j["key"]);
    }
    catch(std::exception& e) // 捕获异常
    {
        LOG_ERROR("GenKeyFd Parse Failed:", e.what());
    }

}

void DataHandler::onReadIdFdDecodeFinish(const ByteArray &data) {
    try
    {
        auto j = json::parse( Encryptor::decrypt(data.toString()));  // 从字符串反序列化
        LOG_INFO("ReadIdFd Parse Success:", j["id"]);
    }
    catch(std::exception& e) // 捕获异常
    {
        LOG_ERROR("ReadIdFd Parse Failed:", e.what());
    }

}
