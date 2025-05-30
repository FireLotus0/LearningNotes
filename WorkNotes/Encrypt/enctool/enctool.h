#pragma once

#include "encryptdog/encryptdog.h"
#include "http/httpmanager.h"
#include "utils/singleinstance/singleinstance.h"

class EncTool : public SingleInstance<EncTool> {
public:
    EncTool();
    ~EncTool();

    int getSessionKey(char* result);

    int getDogId(char* result);

    int refreshKey(const char* sk, char* newKey);

    int refreshKey(char* newKey);

private:
    void str2char(char* addr, const std::string& data);

private:
    EncryptDog* dog;
    HttpManager* server;
    std::string sessionKey;
    unsigned long dhPubKey;
    unsigned long dhPrivateKey;
    unsigned long sk;
};
