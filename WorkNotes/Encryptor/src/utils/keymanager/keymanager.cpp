#include "keymanager.h"
#include "src/utils/aes/encry_aes.h"
#include "src/utils/logger/logger.h"
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qtimer.h>
#include <qhttpmultipart.h>

namespace ET {
#define READ_SERVER_KEY "http://192.168.1.63:6060/api/login/readkey"
#define SYNC_SESSION_KEY "http://192.168.1.63:6060/api/login/setsessionkey"
#define LOGIN_URL "http://192.168.1.63:6060/api/login/login"


KeyManager::KeyManager(QObject *parent)
        : QObject(parent) {
    devId = qMakePair(PID, VID);
    UsbManager::instance().appendInterestDev({devId});
    UsbManager::instance().startSearch();

//    connect(&UsbManager::instance(), &UsbManager::readKeyFinished, this, [&](const QString &readKey) {
//        key = readKey;
//        emit readKeyFinished(key, readKey.isEmpty() ? (int) ErrCode::KEY_INVALID : (int) ErrCode::SUCCESS);
//    });
//    connect(&UsbManager::instance(), &UsbManager::readIdFinished, this, [&](const QString &readId) {
//        id = readId;
//        emit readIdFinished(id, (int) ErrCode::SUCCESS);
//    });
//    connect(&UsbManager::instance(), &UsbManager::generateKeyFinished, this, [&](const QString &genKey) {
//        key = genKey;
//        emit refreshKeyFinished(key, genKey.isEmpty() ? (int) ErrCode::KEY_INVALID : (int) ErrCode::SUCCESS);
//        LOG_INFO("Generate Key Finished! SessionKey=", key);
//        if (!key.isEmpty()) {
//            sendSessionKeyToSever();
//        }
//    });
//
//    connect(&UsbManager::instance(), &UsbManager::information, this, [&](const QString &info, int errCode) {
//        emit errorOccurred(info, errCode);
//    });
//    connect(&UsbManager::instance(), &UsbManager::connectionChanged, this,
//            [&](uint16_t pid, uint16_t vid, bool connected) {
//                if (pid == devId.first && vid == devId.second) {
//                    if (!connected) {
//                        emit deviceRemoved();
//                        LOG_INFO("Usb Device Disconnect: PID=", pid, "VID=", vid);
//                    } else {
//                        emit deviceAttached();
//                    }
//                }
//            });
}


ErrCode KeyManager::encrypt(QByteArray &res, const QByteArray &content) {
    if (key.isEmpty()) {
        return ErrCode::KEY_INVALID;
    }
    res = AES::aesEncrypt(key, content);
    return ErrCode::SUCCESS;
}

ErrCode KeyManager::decrypt(QByteArray &res, const QByteArray &content) {
    if (key.isEmpty()) {
        return ErrCode::KEY_INVALID;
    }
    res = AES::aesDecrypt(key, content);
    return ErrCode::SUCCESS;
}

QString KeyManager::getToken() {
    return token;
}


void KeyManager::sendPubKeyToServer() {
    privateKey = DHCodec::createPrivateKey();
    publicKey = DHCodec::getPublicKey(privateKey);

    QNetworkRequest request;
    request.setUrl(QUrl(READ_SERVER_KEY));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["pub"] = (qint64) publicKey;
    QJsonDocument jsonDoc(jsonObj);

    QNetworkReply *reply = networkManager.post(request, jsonDoc.toJson());
    reply->setProperty("ServerStage", ServerStage::SEND_PUB_KEY);
    connect(reply, &QNetworkReply::finished, this, [&, reply]() {
        onReplyFinished(reply);
    });
    connect(reply, &QNetworkReply::errorOccurred, this, [&, reply](QNetworkReply::NetworkError error) {
        errorOccurred(reply->errorString(), (int) error);
    });

    LOG_INFO("Start Send DH Pub Key To Server: Pub Key=", publicKey);
}

void KeyManager::sendSessionKeyToSever() {
    LOG_INFO("Start Send Session Key To Server: Session Key:", key);
    QNetworkRequest request;
    request.setUrl(QUrl(SYNC_SESSION_KEY));
    request.setRawHeader("Authorization", "Bearer " + token.toLatin1());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["key"] = key;
    auto content = AES::aesEncrypt(sk, QJsonDocument(jsonObj).toJson());

    QJsonObject dataObj;
    dataObj["data"] = QString(content.toBase64());

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(dataObj).toJson());
    reply->setProperty("ServerStage", ServerStage::SEND_SESSION_KEY);
    connect(reply, &QNetworkReply::finished, this, [&, reply]() {
        onReplyFinished(reply);
    });
    connect(reply, &QNetworkReply::errorOccurred, this, [&, reply](QNetworkReply::NetworkError error) {
        errorOccurred(reply->errorString(), (int) error);
    });
}

void KeyManager::generateKey() {
    LOG_INFO("Start Generate Key");
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        emit refreshKeyFinished("", (int) ErrCode::DEVICE_INVALID);
    } else {
        LOG_INFO("Call Device To Generate Key!");
        UsbManager::instance().generateKey(device, sk);
    }
}

void KeyManager::getKey() {
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        emit readKeyFinished("", (int) ErrCode::DEVICE_INVALID);
    } else if (!key.isEmpty()) {
        emit readKeyFinished(key, (int) ErrCode::SUCCESS);
    } else {
        UsbManager::instance().getKey(device);
    }
}

void KeyManager::getId() {
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        emit readIdFinished("", (int) ErrCode::DEVICE_INVALID);
    } else if (!id.isEmpty()) {
        emit readIdFinished(id, (int) ErrCode::SUCCESS);
    } else {
        UsbManager::instance().getId(device);
    }
}

void KeyManager::onReplyFinished(QNetworkReply *reply) {
    auto step = reply->property("ServerStage").toInt();
    switch (step) {
        case ServerStage::SEND_PUB_KEY: {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isNull()) {
                LOG_ERROR("Parse Json Null!");
            } else {
                auto obj = doc.object();
                if (obj.value("OK").toBool()) {
                    serverPubKey = obj.value("pub").toInt();
                    token = obj.value("token").toString();
                    sk = DHCodec::getSharedKey(serverPubKey, privateKey);
                    LOG_INFO("Send Pub Key To Server Finished:Server Pub Key=", serverPubKey, "token:", token,
                             "Server DH SK=", obj.value("shared").toInt(), "Local DH SK=", sk);
                    generateKey();
                } else {
                    LOG_WARNING("Send Pub Key Server Failed:", reply->errorString());
                }
            }
            break;
        }
        case ServerStage::SEND_SESSION_KEY: {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isNull()) {
                LOG_ERROR("Parse Json Null!");
            } else {
                auto obj = doc.object();
                if (obj.value("OK").toBool()) {
                    auto codeData = QByteArray::fromBase64(obj.value("data").toString().toLatin1());
                    token = QJsonDocument::fromJson(AES::aesDecrypt(sk, codeData)).object().value("token").toString();
                    LOG_INFO("Send Session Key To Server Finished:", token);
                    if (!isUserAuthed) {
                        login();
                    }
                } else {
                    LOG_WARNING("Send Session Key To Server Failed: ", reply->errorString());
                }
            }
            break;
        }
        case ServerStage::LOGIN: {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isNull()) {
                LOG_ERROR("Parse Json Null!");
            } else {
                auto obj = doc.object();
                if (obj.value("OK").toBool()) {
                    auto codeData = QByteArray::fromBase64(obj.value("data").toString().toLatin1());
                    token = QJsonDocument::fromJson(AES::aesDecrypt(key, codeData)).object().value("token").toString();
                    LOG_INFO("Verify User Finished Token=", token);
                    isUserAuthed = true;
                    emit loginFinished(false, (int) ErrCode::SUCCESS);
                } else {
                    emit loginFinished(false, (int) ErrCode::LOGIN_FAILED);
                }
            }
            break;
        }
        default:
            Q_ASSERT(false);
    }
    reply->deleteLater();
}

void KeyManager::refreshKey() {
    sk = 0;
    token.clear();
    key.clear();
    sendPubKeyToServer();
}

void KeyManager::login(const QString &user, const QString &passwd) {
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        emit loginFinished(false, (int) ErrCode::DEVICE_INVALID);
        return;
    } else if (id.isEmpty()) {
        id = device->getIdBlocked();
        if (id.isEmpty()) {
            emit loginFinished(false, (int) ErrCode::READ_ID_FAILED);
            return;
        }
    }
    userName = user;
    password = passwd;
    refreshKey();
}

int KeyManager::getKey(char *result) {
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr || device->getDeviceHandle() == nullptr) {
        return (int) ErrCode::DEVICE_INVALID;
    }
    key.clear();
    key = device->getKeyBlocked();
    if (key.isEmpty()) {
        return (int) ErrCode::KEY_INVALID;
    } else {
        memcpy(result, key.toLatin1().data(), key.size());
        result[key.size()] = '\0';
        return (int) ErrCode::SUCCESS;
    }
}

int KeyManager::refreshKey(const char *skAddr, char *result) {
    auto device = UsbManager::instance().getDevice(devId);
    key.clear();
    sk = QString::fromStdString(std::string(skAddr)).toULong();
    LOG_INFO("sk is ", sk, " sk str:", std::string(skAddr));
    if (device == nullptr || device->getDeviceHandle() == nullptr) {
        return (int) ErrCode::DEVICE_INVALID;
    }
    key = device->refreshKeyBlocked(sk);
    if (key.isEmpty()) {
        return (int) ErrCode::KEY_INVALID;
    } else {
        memcpy(result, key.toLatin1().data(), key.size());
        result[key.size()] = '\0';
        return (int) ErrCode::SUCCESS;
    }
}

int KeyManager::getId(char *result) {
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        return (int) ErrCode::DEVICE_INVALID;
    }
    id = device->getIdBlocked();
    if (id.isEmpty()) {
        return (int) ErrCode::READ_ID_FAILED;
    } else {
        memcpy(result, id.toLatin1().data(), id.size());
        result[id.size()] = '\0';
        return (int) ErrCode::SUCCESS;
    }
}

KeyManager &KeyManager::instance() {
    static KeyManager keyManager;
    return keyManager;
}

void KeyManager::login() {
    LOG_INFO("Start Verify User!");
    QNetworkRequest request;
    request.setUrl(QUrl(LOGIN_URL));
    request.setRawHeader("Authorization", "Bearer " + token.toLatin1());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["username"] = userName;
    jsonObj["passwd"] = password;
    jsonObj["hid"] = id;

    auto content = AES::aesEncrypt(key, QJsonDocument(jsonObj).toJson());

    QJsonObject dataObj;
    dataObj["data"] = QString(content.toBase64());

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(dataObj).toJson());
    reply->setProperty("ServerStage", ServerStage::LOGIN);
    connect(reply, &QNetworkReply::finished, this, [&, reply]() {
        onReplyFinished(reply);
    });
    connect(reply, &QNetworkReply::errorOccurred, this, [&, reply](QNetworkReply::NetworkError error) {
        errorOccurred(reply->errorString(), (int) error);
    });
}

int KeyManager::encrypt(char *res, const char *content) {
    if (key.isEmpty()) {
        return (int) ErrCode::KEY_INVALID;
    }
    auto tmpRes = AES::aesEncrypt(key, QString::fromStdString(std::string(content)).toLatin1());
    memcpy(res, tmpRes.data(), tmpRes.size());
    res[tmpRes.size()] = '\0';
    return (int) ErrCode::SUCCESS;
}

int KeyManager::decrypt(char *res, const char *content) {
    if (key.isEmpty()) {
        return (int) ErrCode::KEY_INVALID;
    }
    auto tmpRes = AES::aesDecrypt(key, QString::fromStdString(std::string(content)).toLatin1());
    memcpy(res, tmpRes.data(), tmpRes.size());
    res[tmpRes.size()] = '\0';
    return (int) ErrCode::SUCCESS;
}

int KeyManager::getToken(char *res) {
    if (token.isEmpty()) {
        return (int) ErrCode::TOKEN_INVALID;
    }
    memcpy(res, token.toLatin1().data(), token.size());
    return (int) ErrCode::SUCCESS;
}

int KeyManager::refreshKey(char *result) {
    sk = 0;
    token.clear();
    key.clear();
    return sendPubKeyToServerSync();
}

int KeyManager::sendPubKeyToServerSync() {
    privateKey = DHCodec::createPrivateKey();
    publicKey = DHCodec::getPublicKey(privateKey);

    QNetworkRequest request;
    request.setUrl(QUrl(READ_SERVER_KEY));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["pub"] = (qint64) publicKey;
    QJsonDocument jsonDoc(jsonObj);

    QNetworkReply *reply = networkManager.post(request, jsonDoc.toJson());
    reply->setProperty("ServerStage", ServerStage::SEND_PUB_KEY);
    reply->waitForReadyRead(60000);
    auto rt = onReplyFinishedSync(reply);
    if(rt == (int)ErrCode::SUCCESS) {
        rt = generateKeySync();
        if(rt == (int)ErrCode::SUCCESS) {
            rt = sendSessionKeyToSeverSync();
            if(rt == (int)ErrCode::SUCCESS) {
                rt = loginSync();
            }
        }
    }
    return rt;
}

int KeyManager::onReplyFinishedSync(QNetworkReply *reply) {
    if(!reply->canReadLine()) {
        return (int)ErrCode::NETWORK_REPLY_TIMEOUT;
    }
    auto step = reply->property("ServerStage").toInt();
    auto doc = QJsonDocument::fromJson(reply->readAll());
    int rt = -1;
    if (doc.isNull()) {
        LOG_ERROR("Parse Json Null! Reply  Error:", reply->errorString());
        reply->deleteLater();
        return (int) ErrCode::PARSE_JSON_FAILED;
    }
    switch (step) {
        case ServerStage::SEND_PUB_KEY: {
            auto obj = doc.object();
            if (obj.value("OK").toBool()) {
                serverPubKey = obj.value("pub").toInt();
                token = obj.value("token").toString();
                sk = DHCodec::getSharedKey(serverPubKey, privateKey);
                LOG_INFO("Send Pub Key To Server Finished:Server Pub Key=", serverPubKey, "token:", token,
                         "Server DH SK=", obj.value("shared").toInt(), "Local DH SK=", sk);
                rt = (int) ErrCode::SUCCESS;
            } else {
                LOG_WARNING("Send Pub Key Server Failed:", reply->errorString());
                rt = (int)ErrCode::SERVER_ERROR;
            }
            break;
        }
        case ServerStage::SEND_SESSION_KEY: {
            auto obj = doc.object();
            if (obj.value("OK").toBool()) {
                auto codeData = QByteArray::fromBase64(obj.value("data").toString().toLatin1());
                token = QJsonDocument::fromJson(AES::aesDecrypt(sk, codeData)).object().value("token").toString();
                LOG_INFO("Send Session Key To Server Finished:", token);
                rt = (int) ErrCode::SUCCESS;
            } else {
                LOG_WARNING("Send Session Key To Server Failed: ", reply->errorString());
                rt = (int)ErrCode::SERVER_ERROR;
            }
            break;
        }
        case ServerStage::LOGIN: {
            auto obj = doc.object();
            if (obj.value("OK").toBool()) {
                auto codeData = QByteArray::fromBase64(obj.value("data").toString().toLatin1());
                token = QJsonDocument::fromJson(AES::aesDecrypt(key, codeData)).object().value("token").toString();
                LOG_INFO("Verify User Finished Token=", token);
                isUserAuthed = true;
                rt = (int)ErrCode::SUCCESS;
            } else {
                rt = (int)ErrCode::SERVER_ERROR;
            }
            break;
        }
        default:
            Q_ASSERT(false);
    }
    reply->deleteLater();
    return rt;
}

int KeyManager::generateKeySync() {
    LOG_INFO("Start Generate Key");
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        return (int) ErrCode::DEVICE_INVALID;
    } else {
        LOG_INFO("Call Device To Generate Key!");
        char tmpKey[128];
        auto rt = refreshKey(QString::number(sk).toStdString().c_str(), tmpKey);
        if (rt == (int) ErrCode::SUCCESS) {
            key = QString::fromStdString(std::string(tmpKey));
            LOG_INFO("Device Generate Key Finish Key=", key);
        }
        return rt;
    }
}

int KeyManager::sendSessionKeyToSeverSync() {
    LOG_INFO("Start Send Session Key To Server: Session Key:", key);
    QNetworkRequest request;
    request.setUrl(QUrl(SYNC_SESSION_KEY));
    request.setRawHeader("Authorization", "Bearer " + token.toLatin1());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["key"] = key;
    auto content = AES::aesEncrypt(sk, QJsonDocument(jsonObj).toJson());

    QJsonObject dataObj;
    dataObj["data"] = QString(content.toBase64());

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(dataObj).toJson());
    reply->setProperty("ServerStage", ServerStage::SEND_SESSION_KEY);
    reply->waitForReadyRead(60000);
    auto rt = onReplyFinishedSync(reply);
    if (rt == (int) ErrCode::SUCCESS) {
        LOG_INFO("Send Session Key To Server Finish! Token=", token);
    } else {
        LOG_INFO("Send Session Key To Server Error!");
    }
    return rt;
}

int KeyManager::loginSync() {
    LOG_INFO("Start Verify User!");
    QNetworkRequest request;
    request.setUrl(QUrl(LOGIN_URL));
    request.setRawHeader("Authorization", "Bearer " + token.toLatin1());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["username"] = userName;
    jsonObj["passwd"] = password;
    jsonObj["hid"] = id;

    auto content = AES::aesEncrypt(key, QJsonDocument(jsonObj).toJson());

    QJsonObject dataObj;
    dataObj["data"] = QString(content.toBase64());

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(dataObj).toJson());
    reply->setProperty("ServerStage", ServerStage::LOGIN);
    reply->waitForReadyRead(60000);
    auto rt = onReplyFinishedSync(reply);
    if (rt == (int) ErrCode::SUCCESS) {
        LOG_INFO("Login Finished Token=", token);
    } else {
        LOG_INFO("Login Failed: token=", token, "user:", QString(QJsonDocument(jsonObj).toJson()), "秘钥：", key, " AES Content:", QString(content.toHex()),
                 " Data:", QString(QJsonDocument(dataObj).toJson()));
    }
    return rt;
}

int KeyManager::login(const char *user, const char *passwd) {
    isUserAuthed = false;
    auto device = UsbManager::instance().getDevice(devId);
    if (device == nullptr) {
        return (int)ErrCode::DEVICE_INVALID;
    } else if (id.isEmpty()) {
        id = device->getIdBlocked();
        if (id.isEmpty()) {
           return (int)ErrCode::READ_ID_FAILED;
        }
    }
    userName = QString::fromStdString(std::string(user));
    password = QString::fromStdString(std::string(passwd));
    sk = 0;
    token.clear();
    key.clear();
    return sendPubKeyToServerSync();
}


}
