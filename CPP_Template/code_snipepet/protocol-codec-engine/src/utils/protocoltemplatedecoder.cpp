#include "utils/protocoltemplatedecoder.h"

#include "flagdata/dataheader.h"
#include "flagdata/datacontent.h"
#include "flagdata/datasize.h"
#include "flagdata/dataverify.h"
#include "flagdata/dataend.h"

#include "protocolexception.h"

#include <qdebug.h>

PROTOCOL_CODEC_NAMESPACE_BEGIN

QList<QSharedPointer<ProtocolFlagData>> ProtocolTemplateDecoder::parse(const QString &templateStr, bool contentRequired) {
    QList<QSharedPointer<ProtocolFlagData>> data;

    auto segments = parseToSegment(templateStr);
    for (const auto& segment : segments) {
        if (segment.type == "H") {
            if (contentRequired && segment.data.isEmpty()) {
                throw ProtocolException("header required content!");
            }
            data.append(QSharedPointer<ProtocolFlagDataHeader>::create(QByteArray::fromHex(segment.data.toLatin1())));
        } else if (segment.type == "S") {
            if (contentRequired && segment.data.isEmpty()) {
                throw ProtocolException("size required content!");
            }
            data.append(QSharedPointer<ProtocolFlagDataSize>::create(segment.data.toInt()));
        } else if (segment.type == "C") {
            data.append(QSharedPointer<ProtocolFlagDataContent>::create());
        } else if (segment.type == "V") {
            if (contentRequired) {
                if (segment.data.toUpper() == "CRC16") {
                    data.append(QSharedPointer<ProtocolFlagDataVerify>::create(ProtocolFlagDataVerify::Crc16));
                } else if (segment.data.toUpper() == "SUM8") {
                    data.append(QSharedPointer<ProtocolFlagDataVerify>::create(ProtocolFlagDataVerify::Sum8));
                } else if (segment.data.toUpper() == "SUM16") {
                    data.append(QSharedPointer<ProtocolFlagDataVerify>::create(ProtocolFlagDataVerify::Sum16));
                } else {
                    throw ProtocolException("unknown verify type:" + segment.data);
                }
            }
        } else if (segment.type == "E") {
            if (contentRequired && segment.data.isEmpty()) {
                throw ProtocolException("tail required content!");
            }
            data.append(QSharedPointer<ProtocolFlagDataEnd>::create(QByteArray::fromHex(segment.data.toLatin1())));
        }
    }

    return data;
}

QList<ProtocolTemplateDecoder::Segment> ProtocolTemplateDecoder::parseToSegment(const QString &str) {
    QList<Segment> segments;

    Segment lastSegment;
    int len = str.length();
    for (int i = 0; i < len; i++) {
        auto c = str.at(i);

        if (isLetter(c)) {
            if (lastSegment.isContentBegin) {
                lastSegment.data.append(c);
            } else {
                lastSegment.type = c;
                lastSegment.data = QString();
                segments << lastSegment;
            }
            continue;
        }

        if (isNumber(c)) {
            if (lastSegment.isContentBegin) {
                lastSegment.data.append(c);
            } else {
                if (!lastSegment.type.isEmpty()) {
                    segments.last().data.append(c);
                } else {
                    throw ProtocolException(QLatin1String("number begin with null type:") + c);
                }
            }
            continue;
        }

        if (isComment(c)) {
            if (c == "(") {
                lastSegment.type = QString();
                lastSegment.data = QString();
                lastSegment.isContentBegin = true;
            } else {
                if (segments.isEmpty()) {
                    throw ProtocolException(QLatin1String("content with null type:") + lastSegment.data);
                } else {
                    segments.last().data = lastSegment.data;
                }
                lastSegment.isContentBegin = false;
            }
            continue;
        }

        throw ProtocolException(QLatin1String("unknown snippet char:") + c);
    }

    return segments;
}

bool ProtocolTemplateDecoder::isLetter(const QChar &c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool ProtocolTemplateDecoder::isNumber(const QChar& c) {
    return (c >= '0' && c <= '9') || c == '.' || c == '-';
}

bool ProtocolTemplateDecoder::isComment(const QChar &c) {
    return c == '(' || c == ')';
}

PROTOCOL_CODEC_NAMESPACE_END
