#pragma once

#include <qobject.h>

#include "global.h"
#include "../utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolFlagDataHeader : public ProtocolFlagData {
public:
    explicit ProtocolFlagDataHeader(QByteArray data);

    QString dataToString() override;

    bool verify(char *data, int offset, int maxSize) override;

    void doFrameOffset(int &offset) override;

    QSharedPointer<ProtocolFlagData> copy() const override;

public:
    QByteArray target;
};

PROTOCOL_CODEC_NAMESPACE_END