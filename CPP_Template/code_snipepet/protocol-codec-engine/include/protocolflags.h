#pragma once

#include "global.h"

#include <qdebug.h>

PROTOCOL_CODEC_NAMESPACE_BEGIN

enum class ProtocolFlag {
    Flag_Header = 0,
    Flag_Size,
    Flag_Content,
    Flag_Verify,
    Flag_End,

    Flag_Max,
};

inline QDebug operator<<(QDebug debug, ProtocolFlag flag) {
    switch (flag) {
        case ProtocolFlag::Flag_Header:
            debug << "Flag_Header";
            break;
        case ProtocolFlag::Flag_Size:
            debug << "Flag_Size";
            break;
        case ProtocolFlag::Flag_Content:
            debug << "Flag_Content";
            break;
        case ProtocolFlag::Flag_Verify:
            debug << "Flag_Verify";
            break;
        case ProtocolFlag::Flag_End:
            debug << "Flag_End";
            break;
        default:
            break;
    }
    return debug;
}

PROTOCOL_CODEC_NAMESPACE_END