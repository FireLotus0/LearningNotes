#include "httpmanager.h"
#include "utils/logger/logger.h"
#include "utils/encrypt/encryptor.h"
#include "bean/common_type.h"

#define SERVER_ADDR "192.168.1.63"
#define SERVER_PORT 6060
#define GET_PUB_KEY_URL "/api/login/readkey"
#define SEND_SESSION_KEY_URL "/api/login/setsessionkey"
#define LOGIN_URL "/api/login/login"

HttpManager::HttpManager() {
    client = new httplib::Client(SERVER_ADDR, SERVER_PORT);
}

HttpManager::~HttpManager() {
    delete client;
}

std::string HttpManager::getToken() const {
    return token;
}

int HttpManager::login(const std::string &user, const std::string &passwd, const std::string& dogId) {
    return 0;
}

unsigned long HttpManager::getServerPubKey(unsigned long clientPubKey) {
    json obj;
    obj["pub"] = clientPubKey;
    auto result = client->Post(GET_PUB_KEY_URL, obj.dump(), "application/json");
    if(checkResult(result)) {
        auto body = parseResponse(result.value().body);
        if(!body.is_null()) {
            LOG_DEBUG("GET server pub key response: ", body.dump(), " key=");
            if(body.value("OK", false)) {
                token = body.value("token", "");
                return body.value("pub", 0UL);
            }
        }
    }
    return 0;
}

int HttpManager::sendSessionKey(unsigned long sk, const std::string& sessionKey) {
    assert(sessionKey.size() == 32);
    auto cipherKey = Encryptor::encrypt(sk, sessionKey);
    auto base64Key = Encryptor::toBase64(cipherKey);
    LOG_DEBUG("Session Key=", sessionKey, "Cipher Key=", cipherKey, "base64Key=", base64Key, "fromBase64Key=", Encryptor::fromBase64(base64Key), " decode cipher key=", Encryptor::decrypt(sk, cipherKey));
    json obj;
    obj["data"] = base64Key;
    client->set_bearer_token_auth(token);
    auto result = client->Post(SEND_SESSION_KEY_URL, obj.dump(), "application/json");
    if(checkResult(result)) {
        auto body = parseResponse(result.value().body);
        if(!body.is_null()) {
            LOG_DEBUG("Send session key response: ", body.dump());
            if(body.value("OK", false)) {
                auto cipherRes= Encryptor::fromBase64(body["data"]);
                auto resObj = parseResponse(Encryptor::decrypt(sk, cipherRes));
                if(!resObj.is_null()) {
                    token = resObj["token"];
                    return ErrorCode::OK;
                }
            }
        }
    }

    return ErrorCode::SEND_SESSION_KEY_RESPONSE_FAIL;
}

bool HttpManager::checkResult(const httplib::Result &result) {
    if(!result) {
        LOG_ERROR("HTTP ERROR:", httplib::to_string(result.error()));
        return false;
    }
    return true;
}

json HttpManager::parseResponse(const std::string &data) {
    try
    {
        auto j = json::parse(data);
        return j;
    }
    catch(std::exception& e) // 捕获异常
    {
        LOG_ERROR("ReadIdFd Parse Failed:", e.what());
        return {};
    }
}



