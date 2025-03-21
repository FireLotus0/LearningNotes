#pragma once

#include "../protocolflags.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolFlagData;
class ProtocolFlagReaderInterface {
public:
    virtual QSharedPointer<ProtocolFlagData> readFlag(ProtocolFlag flag) = 0;
};

class ProtocolFlagData {
public:
    explicit ProtocolFlagData(ProtocolFlag flag);

    virtual QString dataToString();

    virtual bool verify(char* data, int offset, int maxSize) = 0;

    virtual void doFrameOffset(int& offset) = 0;

    virtual QSharedPointer<ProtocolFlagData> copy() const = 0;

    void setFlagReader(ProtocolFlagReaderInterface* reader);

public:
    ProtocolFlag flag;
    ProtocolFlagReaderInterface* flagReader = nullptr;
};

inline QDebug operator<<(QDebug debug, ProtocolFlagData* data) {
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "[";
    debug << data->flag;
    debug << "]";
    auto str = data->dataToString();
    if (!str.isEmpty()) {
        debug << ": ";
        debug << str;
    }
    return debug;
}

PROTOCOL_CODEC_NAMESPACE_END