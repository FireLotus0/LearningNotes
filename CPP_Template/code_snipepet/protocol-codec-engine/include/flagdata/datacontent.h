#pragma once

#include <qobject.h>

#include "global.h"
#include "../utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolFlagDataSize;
class ProtocolFlagDataVerify;
class ProtocolFlagDataEnd;

class ProtocolFlagDataContent : public ProtocolFlagData {
public:
    explicit ProtocolFlagDataContent();

    bool verify(char *data, int offset, int maxSize) override;

    void doFrameOffset(int &offset) override;

    void setDependentFlag(const QSharedPointer<ProtocolFlagDataSize>& sizeFlag,
                          const QSharedPointer<ProtocolFlagDataVerify>& verifyFlag,
                          const QSharedPointer<ProtocolFlagDataEnd>& endFlag);

    QSharedPointer<ProtocolFlagData> copy() const override;

public:
    QByteArray contentData;

private:
    QSharedPointer<ProtocolFlagDataSize> mSizeFlag;
    QSharedPointer<ProtocolFlagDataVerify> mVerifyFlag;
    QSharedPointer<ProtocolFlagDataEnd> mEndFlag;
};

PROTOCOL_CODEC_NAMESPACE_END