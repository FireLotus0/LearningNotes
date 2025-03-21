#pragma once

#include <qobject.h>

#include "global.h"
#include "protocoltypecodec.h"

#include "decoder/protocoldecoder.h"
#include "encoder/protocolencoder.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolCodecEngine : public QObject {
public:
    explicit ProtocolCodecEngine(QObject *parent = nullptr);

    /**
     * @brief 定义协议，例如：H(5AFF)S2CV(CRC16)E(FE) （括号表示具体值，数字表示字节数）
     * H：header/帧头
     * S：size/大小
     * C：content/内容（隐式包含数据类型）
     * V：verify/校验类型
     * E：end/帧尾
     * @param templateStr
     */
    void frameDeclare(const QString& templateStr);

    /**
     * @brief 设置校验Flag（需要先调用frameDeclare），例如：HSC，表示校验帧头、大小、内容所有字节数
     * @param flagStr
     */
    void setVerifyFlags(const QString& flagStr);

    /**
     * @brief 设置类型编解码使用的字节数，默认2字节，最大4字节
     * @param size
     */
    void setTypeEncodeByteSize(int size);

    /**
     * @brief 注册类型和解析回调
     * @tparam T 类型
     * @tparam K 回调类类型
     * @param context
     * @param callback
     * @param codec 编码器
     */
    template<typename Codec, typename T, typename K>
    void registerType(K* context, void(K::*callback)(const T& t)) {
        decoder.addType(T::Type, [context, callback] (const QByteArray& content) {
            (context->*callback)(Codec().decode(content));
        });
        Q_ASSERT_X(encoder.getCodec(T::Type) == nullptr, "ProtocolCodecEngine::registerType", "Type already registered!");
        encoder.addType(T::Type, new Codec);
    }

    /**
     * @brief 注册类型，不使用其解码器，只编码
     * @tparam T
     * @tparam Codec
     */
    template<typename T, template<typename> class Codec>
    void registerType() {
        Q_ASSERT_X(encoder.getCodec(T::Type) == nullptr, "ProtocolCodecEngine::registerType", "Type already registered!");
        encoder.addType(T::Type, new Codec<T>);
    }

    /**
     * @brief 注册压缩编码器，只编码
     * @tparam T
     * @tparam Codec
     */
    template<typename T, template<typename> class Codec, int Level = -1>
    void registerCompressEncoder() {
        Q_ASSERT_X(encoder.getCodec(T::Type) == nullptr, "ProtocolCodecEngine::registerType", "Type already registered!");
        encoder.addType(T::Type, new CompressCodec<T, Codec, Level>);
    }

    /**
     * @brief 注册类型，仅定义命令
     * @tparam I
     * @tparam T
     * @param context
     * @param callback
     */
    template<int I, typename T>
    void registerType(T* context, void(T::*callback)()) {
        Q_ASSERT_X(!decoder.checkRegistered(I), "ProtocolCodecEngine::registerType", "Type already registered!");
        decoder.addType(I, [context, callback] (const QByteArray& content) {
            (context->*callback)();
        });
    }

    /**
     * @brief 数据编码
     * @tparam T
     * @param data
     * @return
     */
    template<typename T>
    QByteArray encode(const T& data) {
        auto codec = encoder.getCodec(T::Type);
        auto content = dynamic_cast<ProtocolTypeCodec<T>*>(codec)->encode(data);
        return encoder.encodeFrame(content, T::Type);
    }

    /**
     * @brief 数据编码，仅编码
     * @tparam I
     * @return
     */
    template<int I>
    QByteArray encode() {
        return encoder.encodeFrame(QByteArray(), I);
    }

    /**
     * @brief 添加新的待解析缓存
     * @param buffer
     */
    void appendBuffer(const QByteArray& buffer);

private:
    ProtocolDecoder decoder;
    ProtocolEncoder encoder;
};

PROTOCOL_CODEC_NAMESPACE_END