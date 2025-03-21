#include "utils/protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

ProtocolFlagData::ProtocolFlagData(ProtocolFlag flag)
    : flag(flag)
{}

QString ProtocolFlagData::dataToString() {
    return {};
}

void ProtocolFlagData::setFlagReader(ProtocolFlagReaderInterface *reader) {
    flagReader = reader;
}

PROTOCOL_CODEC_NAMESPACE_END



