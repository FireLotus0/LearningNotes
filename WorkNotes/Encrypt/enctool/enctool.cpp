#include "enctool.h"

EncTool::EncTool() {
    dog = EncryptDog::instance();
    server = HttpManager::instance();
}

EncTool::~EncTool() {
    delete dog;
    delete server;
}

int EncTool::getDogId(char *result) {
    auto data = dog->readId();
    if(data.first != ErrorCode::OK) {
        return data.first;
    }
    str2char(result, data.second);
    return ErrorCode::OK;
}

int EncTool::getSessionKey(char *result) {
    if(sessionKey.empty()) {
        auto data = dog->readKey();
        if(data.first == ErrorCode::OK) {
            sessionKey = data.second;
        }
    }
    if(sessionKey.empty()) {
        return ErrorCode::KEY_EMPTY_FROM_DOG;
    }
    str2char(result, sessionKey);
    return ErrorCode::OK;
}

void EncTool::str2char(char *addr, const std::string &data) {
    auto sz = data.size();
    memcpy(addr, data.data(), sz);
    addr[sz] = '\0';
}

int EncTool::refreshKey(char *newKey) {
    dhPrivateKey = Encryptor::DHCodec::createPrivateKey();
    dhPubKey = Encryptor::DHCodec::getPublicKey(dhPrivateKey);
    auto serverPubKey = server->getServerPubKey(dhPubKey);
    if(serverPubKey == 0) {
        return ErrorCode::SERVER_PUB_KEY_INVALID;
    }
    sk = Encryptor::DHCodec::getSharedKey(serverPubKey, dhPrivateKey);
    LOG_DEBUG("DH SK=", sk);
    auto generateKey = dog->refreshKey(std::to_string(sk));
    if(generateKey.first != ErrorCode::OK) {
        return generateKey.first;
    }
    sessionKey = generateKey.second;
    auto rt = server->sendSessionKey(sk, sessionKey);
    if(rt != ErrorCode::OK) {
        return rt;
    }
    str2char(newKey, sessionKey);
    return ErrorCode::OK;
}

int EncTool::refreshKey(const char *sk, char *newKey) {
    return 0;
}
