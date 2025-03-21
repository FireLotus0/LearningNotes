#pragma once

#include <qobject.h>

#include "global.h"
#include "../utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolFlagDataVerify : public ProtocolFlagData {
public:
    enum VerifyType {
        Crc16,
        Sum8,
        Sum16,
    };

    explicit ProtocolFlagDataVerify(VerifyType verifyType);

    QString dataToString() override;

    bool verify(char *data, int offset, int maxSize) override;

    void doFrameOffset(int &offset) override;

    void setVerifyFlags(const QList<QSharedPointer<ProtocolFlagData>>& flags);

    QByteArray getVerifyCode(const QByteArray& buff, int contentOffset);

    QSharedPointer<ProtocolFlagData> copy() const override;

public:
    VerifyType verifyType;
    int byteSize;

private:
    QList<QSharedPointer<ProtocolFlagData>> verifyFlags;
};

PROTOCOL_CODEC_NAMESPACE_END