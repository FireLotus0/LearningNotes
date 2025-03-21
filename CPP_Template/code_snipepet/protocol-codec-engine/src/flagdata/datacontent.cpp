#include "flagdata/datacontent.h"

#include "flagdata/datasize.h"
#include "flagdata/dataverify.h"
#include "flagdata/dataend.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

ProtocolFlagDataContent::ProtocolFlagDataContent()
    : ProtocolFlagData(ProtocolFlag::Flag_Content)
{}

bool ProtocolFlagDataContent::verify(char *data, int offset, int maxSize) {
    contentData = QByteArray(data + offset, mSizeFlag->dataSize);
    return true;
}

void ProtocolFlagDataContent::doFrameOffset(int &offset) {
    offset += mSizeFlag->dataSize;
    offset += mVerifyFlag->byteSize;
    if (!mEndFlag.isNull()) {
        offset += mEndFlag->target.size();
    }
}

void ProtocolFlagDataContent::setDependentFlag(const QSharedPointer<ProtocolFlagDataSize> &sizeFlag,
                                               const QSharedPointer<ProtocolFlagDataVerify> &verifyFlag,
                                               const QSharedPointer<ProtocolFlagDataEnd> &endFlag
) {
    mSizeFlag = sizeFlag;
    mVerifyFlag = verifyFlag;
    mEndFlag = endFlag;
}

QSharedPointer<ProtocolFlagData> ProtocolFlagDataContent::copy() const  {
    auto newFlag = QSharedPointer<ProtocolFlagDataContent>::create();
    newFlag->contentData = contentData;
    return newFlag;
}

PROTOCOL_CODEC_NAMESPACE_END