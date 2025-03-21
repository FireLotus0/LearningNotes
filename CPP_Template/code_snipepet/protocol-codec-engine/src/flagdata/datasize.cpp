#include "flagdata/datasize.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

ProtocolFlagDataSize::ProtocolFlagDataSize(int byteSize)
    : ProtocolFlagData(ProtocolFlag::Flag_Size)
    , byteSize(byteSize)
    , dataSize(0)
{}

QString ProtocolFlagDataSize::dataToString() {
    return QString::number(byteSize);
}

bool ProtocolFlagDataSize::verify(char *data, int offset, int maxSize) {
    if (offset + byteSize > maxSize) {
        return false;
    }
    //小端序
    dataSize = 0;
    for (int i = 0; i < byteSize; i++) {
        dataSize |= (data[offset + i] & 0xff) << (8 * i);
    }
    return dataSize >= 0;
}

void ProtocolFlagDataSize::doFrameOffset(int &offset) {
    offset += byteSize;
}

QSharedPointer<ProtocolFlagData> ProtocolFlagDataSize::copy() const {
    auto newFlag = QSharedPointer<ProtocolFlagDataSize>::create(byteSize);
    newFlag->dataSize = dataSize;
    return newFlag;
}

PROTOCOL_CODEC_NAMESPACE_END
