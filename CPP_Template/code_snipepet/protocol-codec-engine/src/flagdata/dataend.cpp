#include "flagdata/dataend.h"

#include "flagdata/datasize.h"
#include "flagdata/dataverify.h"

#include "protocolexception.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

ProtocolFlagDataEnd::ProtocolFlagDataEnd(QByteArray data)
    : ProtocolFlagData(ProtocolFlag::Flag_End)
    , target(std::move(data))
{}

QString ProtocolFlagDataEnd::dataToString() {
    return target.toHex(' ');
}

bool ProtocolFlagDataEnd::verify(char *data, int offset, int maxSize) {
    auto endOffset = offset + mSizeFlag->dataSize + mVerifyFlag->byteSize;
    if (endOffset <= offset) {
        return false;
    }
    if (endOffset + target.size() > maxSize) {
        return false;
    }
    return QByteArray(data + endOffset, target.size()) == target;
}

void ProtocolFlagDataEnd::doFrameOffset(int &offset) {
}

void ProtocolFlagDataEnd::setDependentFlag(const QSharedPointer<ProtocolFlagDataSize> &sizeFlag,
                                           const QSharedPointer<ProtocolFlagDataVerify> &verifyFlag)
{
    mSizeFlag = sizeFlag;
    mVerifyFlag = verifyFlag;
    if (mSizeFlag.isNull() || mVerifyFlag.isNull()) {
        throw ProtocolException("frame end decode require 'size' and 'verify'!");
    }
}

QSharedPointer<ProtocolFlagData> ProtocolFlagDataEnd::copy() const {
    return QSharedPointer<ProtocolFlagDataEnd>::create(target);
}

PROTOCOL_CODEC_NAMESPACE_END