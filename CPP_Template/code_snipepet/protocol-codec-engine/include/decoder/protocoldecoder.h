#pragma once

#include <qobject.h>
#include <qhash.h>

#include "../protocolcodecinterface.h"
#include "../utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolDecoder : public ProtocolCodecInterface {
public:
    using ProtocolCodecInterface::ProtocolCodecInterface;

    void setFlags(const QList<QSharedPointer<ProtocolFlagData>>& flags) override;

    void addType(int type, const std::function<void(const QByteArray&)>& decoder);

    bool checkRegistered(int type) const;

    void addBuffer(const QByteArray& buffer);

private:
    QHash<int, std::function<void(const QByteArray&)>> typeDecoders;

    QByteArray bufferCache;
};

PROTOCOL_CODEC_NAMESPACE_END