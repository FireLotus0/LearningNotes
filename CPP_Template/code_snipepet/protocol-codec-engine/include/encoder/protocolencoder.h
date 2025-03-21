#pragma once

#include <qobject.h>
#include <qhash.h>

#include "../protocolcodecinterface.h"
#include "../protocoltypecodec.h"
#include "../utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolEncoder : public ProtocolCodecInterface {
public:
    using ProtocolCodecInterface::ProtocolCodecInterface;
    ~ProtocolEncoder() override;

    void addType(int type, ProtocolTypeCodecInterface* codec);

    ProtocolTypeCodecInterface* getCodec(int type);

    QByteArray encodeFrame(const QByteArray& content, int dataType);

private:
    QHash<int, ProtocolTypeCodecInterface*> typeEncoders;
};

PROTOCOL_CODEC_NAMESPACE_END