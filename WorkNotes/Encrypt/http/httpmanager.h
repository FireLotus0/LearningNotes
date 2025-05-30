#pragma once
#include "httplib.h"
#include "utils/json/json.h"
#include "utils/singleinstance/singleinstance.h"

using namespace nlohmann;

class HttpManager : public SingleInstance<HttpManager>{
public:
    HttpManager();
    ~HttpManager();

    int login(const std::string& user, const std::string& passwd, const std::string& dogId);

    // 获取服务器DH公钥
    unsigned long getServerPubKey(unsigned long clientPubKey);

    // 发送会话秘钥到服务器
    int sendSessionKey(unsigned long sk, const std::string& sessionKey);

    std::string getToken() const;

private:
    bool checkResult(const httplib::Result& result);

    json parseResponse(const std::string& data);
private:
    std::string token;
    httplib::Client* client;
};
