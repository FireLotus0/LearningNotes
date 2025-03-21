#pragma once

#include <qobject.h>

#include "global.h"
#include "utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolCodecInterface : public QObject, public ProtocolFlagReaderInterface {
public:
    explicit ProtocolCodecInterface(QObject *parent = nullptr);

    virtual void setFlags(const QList<QSharedPointer<ProtocolFlagData>>& flags);

    void setVerifyFlags(const QList<QSharedPointer<ProtocolFlagData>>& flags);

    QSharedPointer<ProtocolFlagData> readFlag(ProtocolFlag flag) override;

    void setTypeByteSize(int size);

protected:
    QSharedPointer<ProtocolFlagData> enumFlags[(int)ProtocolFlag::Flag_Max];
    QList<QSharedPointer<ProtocolFlagData>> protocolFlags;

    int mTypeByteSize;

protected:
    template<typename T>
    QSharedPointer<T> get(ProtocolFlag flag) {
        return qSharedPointerCast<T>(enumFlags[(int)flag]);
    }
};

PROTOCOL_CODEC_NAMESPACE_END