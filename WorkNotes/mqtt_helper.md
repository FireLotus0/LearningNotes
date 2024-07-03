### MQTT辅助类：使用QT官方的QTMQTT
* mqtt数据编码，解码器
1. 接口文件：mqttcodeinterface.h
```cpp
#pragma once

#include "datautil/datakey.h"

#include <qjsondocument.h>
#include <qobject.h>

using namespace QDataUtil;

class MqttEncodeInterface
{
public:
    explicit MqttEncodeInterface(QString topic = "") : topic(std::move(topic)) {}
    virtual ~MqttEncodeInterface() = default;

    QString topic;
};

template<typename T>
class MqttTypeEncoder : public MqttEncodeInterface
{
public:
    using MqttEncodeInterface::MqttEncodeInterface;

    virtual QByteArray encode(const T& data) = 0;
};

template<typename T>
class MqttJsonCodec : public MqttTypeEncoder<T> {
public:
    using MqttTypeEncoder<T>::MqttTypeEncoder;

    T decode(const QByteArray &content) {
        auto doc = QJsonDocument::fromJson(content);
        if (doc.isNull()) {
            return T();
        }
        return T::fromJson(doc);
    }

    QByteArray encode(const T &data) override {
        QJsonDocument doc = data.toJson();
        return doc.toJson(QJsonDocument::Compact);
    }
};

template<typename T>
struct MqttJsonDataCodec : DataDumpInterface {
    static T fromJson(const QJsonDocument& content) {
        T data;
        auto jObj = content.object();
        data.DataDumpInterface::fromJson(jObj);
        return data;
    }

    QJsonDocument toJson() const {
        T tmp = *dynamic_cast<const T*>(this);
        return QJsonDocument(tmp.dumpToJson());
    }
};

```
2. mqttcode.h
```cpp
#pragma once

#include <qobject.h>
#include <qsharedpointer.h>

#include "mqttclient.h"
#include "mqttcodecinterface.h"

class MqttCodec : public QObject
{
    Q_OBJECT
public:
    explicit MqttCodec(QObject *parent = nullptr);

    void init(const MqttInitParams &initParams);

    QString getDeviceId() const;

    template<typename Decoder, typename T, typename E>
    void registerType(const QString& topic, T* context, void(T::*callback)(const E& e, const QString& topicId)) {
        Q_ASSERT_X(!typeDecoders.contains(topic), "MqttCodec::registerType", "Type already registered!");
        typeDecoders[topic] = [context, callback] (const QByteArray& content, const QString& topicId) {
            (context->*callback)(Decoder().decode(content), topicId);
        };
    }

    template<template<typename> class Encoder, typename E>
    void registerType(const QString& topic) {
        Q_ASSERT_X(!typeEncoders.contains(typeid(E).name()), "MqttCodec::registerType", "Type already registered!");
        typeEncoders[typeid(E).name()] = QSharedPointer<Encoder<E>>::create(topic);
    }

    template<typename E>
    void publish(const E& data) {
        auto codec = dynamic_cast<MqttTypeEncoder<E>*>(typeEncoders[typeid(E).name()].data());
        Q_ASSERT_X(codec != nullptr, "MqttCodec::publish", "Type not registered!");
        publish(codec->topic, codec->encode(data));
    }

    void subscribeAll();

signals:
    void stateChanged(bool connected);
private:
    MqttClient* mqttClient;
    QHash<QString, std::function<void(const QByteArray&, const QString&)>> typeDecoders;
    QHash<QString, QSharedPointer<MqttEncodeInterface>> typeEncoders;

private:
    void receivedHostMessage(const QByteArray& message, const QString& topicName);   //解析订阅的topic收到的消息
    void publish(const QString& topic, const QByteArray& data);
};
```

2. mqttcode.cpp
```cpp
#include "mqttcodec.h"

#include <styledstring.h>

MqttCodec::MqttCodec(QObject *parent)
    : QObject(parent)
{
    mqttClient = new MqttClient(this);
    connect(mqttClient, &MqttClient::recvMsg, this, &MqttCodec::receivedHostMessage);
    connect(mqttClient, &MqttClient::connected, this, &MqttCodec::subscribeAll);
    connect(mqttClient, &MqttClient::stateChanged, this, &MqttCodec::stateChanged);
}

void MqttCodec::init(const MqttInitParams &initParams) {
    mqttClient->init(initParams);
}

void MqttCodec::receivedHostMessage(const QByteArray &message, const QString& topicName) {
    int index = topicName.lastIndexOf("/");
    QString topic, topicId;
    if (index != -1) {
        topic = topicName.mid(0, index + 1);
        topicId = topicName.mid(index + 1);
    } else {
        topic = topicName;
    }
    //find topic by key
    if (typeDecoders.contains(topic)) {
        typeDecoders[topic](message, topicId);
        return;
    }
    //find topic by filter
    for (auto i = typeDecoders.begin(); i != typeDecoders.end(); ++i) {
        QMqttTopicFilter filter(i.key());
        if (filter.match(topicName)) {
            i.value()(message, topicId);
            return;
        }
    }
    qCritical() << styled("cannot find decoder for topic:").r() << topicName;
}

void MqttCodec::subscribeAll() {
    auto keys = typeDecoders.keys();
    for (const auto& key : keys) {
        auto subTopic = mqttClient->client->subscribe(key);
        connect(subTopic, &QMqttSubscription::stateChanged, this, [&, key](QMqttSubscription::SubscriptionState state) {
            if(state == QMqttSubscription::Subscribed) {
                qDebug() << key << "subscribed success!";
            } else if(state == QMqttSubscription::Error) {
                qDebug() << key << "subscribed fail!";
            }
        });
    }
}

void MqttCodec::publish(const QString &topic, const QByteArray &data) {
    mqttClient->client->publish(topic, data);
}

QString MqttCodec::getDeviceId() const {
    return mqttClient->getDeviceId();
}

```

* mqtt客户端封装
1. mqttclient.h
```cpp
#pragma once

#include <qobject.h>
#include <qmqttclient.h>
#include "utils/mqttconfig.h"
struct MqttInitParams {
    QString host;
    int port = 0;
    QString userName;
    QString pwd;
    QString deviceId;

    int keepAlive = 300;
    int autoConnectInterval = 0;
};

class MqttClient : public QObject
{
    Q_OBJECT

public:
    explicit MqttClient(QObject* parent = nullptr);

    void init(const MqttInitParams& initParams);
    void publishMessage(const QString& topic, const QByteArray& data);

    QString getDeviceId() const;
signals:
    void connected();
    void disconnected();
    void recvMsg(const QByteArray& data, const QString& topic);
    void stateChanged(bool connected);

private slots:
    void mqttErrorOccur(QMqttClient::ClientError error);  //mqtt发生错误
    void mqttPingResp();
    void onRecvMsg(const QByteArray& data, const QMqttTopicName &topic);   // 收到消息
    void mqttStateChanged(QMqttClient::ClientState state);
    void mqttDisconnected();

private:
    void connectToHost();

private:
    QMqttClient *client = nullptr;
    MqttInitParams mqttInitParams;

    friend class MqttCodec;
};

extern QByteArray hmacSha1(QByteArray key, const QByteArray &baseString);
```
2. mqttclient.cpp
```cpp
#include "mqttclient.h"

#include <qdatetime.h>
#include <qdebug.h>
#include <styledstring.h>
#include <qcryptographichash.h>
#include <qtimer.h>

QByteArray hmacSha1(QByteArray key, const QByteArray &baseString) {
    int blockSize = 64;            // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) {// if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }
    QByteArray innerPadding(blockSize, char(0x36));// initialize inner padding with char"6"
    QByteArray outerPadding(blockSize, char(0x5c));// initialize outer padding with char"/"
    // ascii characters 0x36 ("6") and 0x5c ("/") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)
    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i);// XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i);// XOR operation between every byte in key and outerpadding, of key length
    }
    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toBase64();
}

MqttClient::MqttClient(QObject* parent)
    : QObject(parent)
{
}

void MqttClient::init(const MqttInitParams& initParams) {
    mqttInitParams = initParams;
    if (client) {
        delete client;
        client = nullptr;
    }
    client = new QMqttClient(this);
    client->setProtocolVersion(QMqttClient::ProtocolVersion::MQTT_3_1_1);
    client->setHostname(initParams.host);
    client->setPort(initParams.port);
    if (!initParams.deviceId.isEmpty()) {
        client->setClientId(initParams.deviceId);
    }
    client->setUsername(initParams.userName);
    client->setPassword(initParams.pwd.toLatin1());

    if (initParams.keepAlive > 0) {
        client->setKeepAlive(initParams.keepAlive);
        client->setAutoKeepAlive(true);
    }

    connect(client, &QMqttClient::connected, this, &MqttClient::connected);
    connect(client, &QMqttClient::disconnected, this, &MqttClient::mqttDisconnected);
    connect(client, &QMqttClient::errorChanged, this, &MqttClient::mqttErrorOccur);
    connect(client, &QMqttClient::pingResponseReceived, this, &MqttClient::mqttPingResp);
    connect(client, &QMqttClient::messageReceived, this, &MqttClient::onRecvMsg);
    connect(client, &QMqttClient::stateChanged, this, &MqttClient::mqttStateChanged);

    connectToHost();
}

void MqttClient::connectToHost() {
    if (client) {
        if (client->state() != QMqttClient::Connected) {
            client->connectToHost();
        }
    }
}

void MqttClient::publishMessage(const QString &topic, const QByteArray &data) {
    client->publish(topic, data);
}

void MqttClient::mqttErrorOccur(QMqttClient::ClientError error) {
    qCritical() << "mqtt error occur:" << error;
}

void MqttClient::mqttPingResp() {
    qDebug() << "mqtt ping resp";
}

void MqttClient::onRecvMsg(const QByteArray& data,const QMqttTopicName &topic) {
    recvMsg(data, topic.name());
}

void MqttClient::mqttStateChanged(QMqttClient::ClientState state) {
    if(state == QMqttClient::ClientState::Connected) {
        qInfo() << "mqtt state connected";
        stateChanged(true);
    } else if(state == QMqttClient::ClientState::Connecting) {
        qInfo() << "mqtt state connecting";
    } else if(state == QMqttClient::ClientState::Disconnected) {
        qInfo() << "mqtt state disconnected";
        stateChanged(false);
    }
}

void MqttClient::mqttDisconnected() {
    emit disconnected();

    if (mqttInitParams.autoConnectInterval > 0) {
        QTimer::singleShot(mqttInitParams.autoConnectInterval, this, &MqttClient::connectToHost);
    }
}

QString MqttClient::getDeviceId() const {
    return mqttInitParams.deviceId;
}

```
