#pragma once

#include <qbytearray.h>
#include <qdatastream.h>
#include <qjsondocument.h>

#include "global.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolTypeCodecInterface {
public:
    virtual ~ProtocolTypeCodecInterface() = default;
};

template<typename T>
class ProtocolTypeCodec : public ProtocolTypeCodecInterface {
public:
    virtual T decode(const QByteArray& content) = 0;
    virtual QByteArray encode(const T& data) = 0;
};

template<typename T>
class JsonCodec : public ProtocolTypeCodec<T> {
public:
    T decode(const QByteArray &content) override {
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
class SerializeCodec : public ProtocolTypeCodec<T> {
public:
    T decode(const QByteArray &content) override {
        QDataStream in(content);
        T data;
        in >> data;
        return data;
    }

    QByteArray encode(const T &data) override {
        QByteArray buffer;
        QDataStream out(&buffer, QIODevice::WriteOnly);
        out << data;
        return buffer;
    }
};

template<typename T>
class BytesCodec : public ProtocolTypeCodec<T> {
public:
    T decode(const QByteArray &content) override {
        return T::fromBytes(content);
    }

    QByteArray encode(const T &data) override {
        return data.toBytes();
    }
};

template<typename T, template<typename> class Codec, int Level = -1>
class CompressCodec : public ProtocolTypeCodec<T> {
public:
    T decode(const QByteArray &content) override {
        auto tmp = qUncompress(content);
        return codec.decode(tmp);
    }

    QByteArray encode(const T &data) override {
        return qCompress(codec.encode(data), Level);
    }

private:
    Codec<T> codec;
};

PROTOCOL_CODEC_NAMESPACE_END