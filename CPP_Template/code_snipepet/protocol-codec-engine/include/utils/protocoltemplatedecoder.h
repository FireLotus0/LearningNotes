#pragma once

#include <qobject.h>
#include <qsharedpointer.h>
#include <qexception.h>

#include "../protocolflags.h"
#include "../global.h"

#include "protocolflagdata.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolTemplateDecoder {
public:
    static QList<QSharedPointer<ProtocolFlagData>> parse(const QString& templateStr, bool contentRequired = true);

private:
    struct Segment {
        QString type;
        QString data;

        bool isContentBegin = false;
    };

    static QList<Segment> parseToSegment(const QString& str);

private:
    static bool isLetter(const QChar& c);
    static bool isNumber(const QChar& c);
    static bool isComment(const QChar& c);
};

PROTOCOL_CODEC_NAMESPACE_END