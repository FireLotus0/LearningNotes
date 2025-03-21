#include "protocolcodecinterface.h"

#include "flagdata/dataheader.h"
#include "flagdata/datacontent.h"
#include "flagdata/datasize.h"
#include "flagdata/dataverify.h"
#include "flagdata/dataend.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

ProtocolCodecInterface::ProtocolCodecInterface(QObject *parent)
    : QObject(parent)
    , mTypeByteSize(2)
{
}

void ProtocolCodecInterface::setFlags(const QList<QSharedPointer<ProtocolFlagData>> &flags) {
    for (const auto& flag : flags) {
        protocolFlags.append(flag->copy());
    }

    for (const auto& flag : protocolFlags) {
        flag->setFlagReader(this);
        enumFlags[(int)flag->flag] = flag;
    }

    auto flagEnd = get<ProtocolFlagDataEnd>(ProtocolFlag::Flag_End);
    if (flagEnd) {
        flagEnd->setDependentFlag(get<ProtocolFlagDataSize>(ProtocolFlag::Flag_Size), get<ProtocolFlagDataVerify>(ProtocolFlag::Flag_Verify));
    }

    auto flagContent = get<ProtocolFlagDataContent>(ProtocolFlag::Flag_Content);
    if (flagContent) {
        flagContent->setDependentFlag(get<ProtocolFlagDataSize>(ProtocolFlag::Flag_Size),
                                      get<ProtocolFlagDataVerify>(ProtocolFlag::Flag_Verify),
                                      get<ProtocolFlagDataEnd>(ProtocolFlag::Flag_End));
    }
}

void ProtocolCodecInterface::setVerifyFlags(const QList<QSharedPointer<ProtocolFlagData>> &flags) {
    auto verifyFlag = get<ProtocolFlagDataVerify>(ProtocolFlag::Flag_Verify);
    if (verifyFlag) {
        verifyFlag->setVerifyFlags(flags);
    }
}

QSharedPointer<ProtocolFlagData> ProtocolCodecInterface::readFlag(ProtocolFlag flag) {
    return enumFlags[(int)flag];
}

void ProtocolCodecInterface::setTypeByteSize(int size) {
    mTypeByteSize = size;
}

PROTOCOL_CODEC_NAMESPACE_END