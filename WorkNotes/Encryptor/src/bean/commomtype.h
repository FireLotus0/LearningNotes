#pragma once

#include <qstring.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

struct ReadKey {
    enum {
        Type = 1
    };

    QString cpuId;
    QString mac;

    //json数据解码
    static ReadKey fromJson(const QJsonDocument& doc) {
        ReadKey tmp;
        tmp.cpuId = doc.object().value("cpuId").toString();
        tmp.mac = doc.object().value("mac").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("cpuId", cpuId);
        obj.insert("mac", mac);
        return QJsonDocument(obj);
    }
};

struct ReadKeyFd {
    enum {
        Type = 4
    };

    QString key;

    //json数据解码
    static ReadKeyFd fromJson(const QJsonDocument& doc) {
        ReadKeyFd tmp;
        tmp.key = doc.object().value("key").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("key", key);
        return QJsonDocument(obj);
    }
};

struct GenKey {
    enum {
        Type = 2
    };

    QString cpuId;
    QString mac;
    QString sk;

    //json数据解码
    static GenKey fromJson(const QJsonDocument& doc) {
        GenKey tmp;
        tmp.cpuId = doc.object().value("cpuId").toString();
        tmp.mac = doc.object().value("mac").toString();
        tmp.sk = doc.object().value("sk").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("cpuId", cpuId);
        obj.insert("mac", mac);
        obj.insert("sk", sk);
        qDebug() << "To Json:" << QJsonDocument(obj).toJson();
        return QJsonDocument(obj);
    }
};

struct GenKeyFd {
    enum {
        Type = 5
    };

    QString key;

    //json数据解码
    static GenKeyFd fromJson(const QJsonDocument& doc) {
        GenKeyFd tmp;
        tmp.key = doc.object().value("key").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("key", key);
        return QJsonDocument(obj);
    }
};

struct ReadId {
    enum {
        Type = 3
    };

    //json数据解码
    static ReadId fromJson(const QJsonDocument& doc) {
        ReadId tmp;
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        return QJsonDocument(obj);
    }
};

struct ReadIdFd {
    enum {
        Type = 6
    };

    QString id;

    static ReadIdFd fromJson(const QJsonDocument& doc) {
        ReadIdFd tmp;
        tmp.id = doc.object().value("id").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("id", id);
        return QJsonDocument(obj);
    }
};