#include "flagdata/dataverify.h"

#include "flagdata/dataheader.h"
#include "flagdata/datasize.h"

PROTOCOL_CODEC_NAMESPACE_BEGIN

/**
 * @brief 和校验
 * @param arr
 * @param lens
 * @return char类型
 */
static uchar sumCheck(const char* arr, int lens) {
    uchar sum = 0;
    for (int i = 0; i < lens; i++) {
        sum += (uchar)*(arr + i);
    }
    return sum;
}

/**
 * @brief 和校验
 * @param arr
 * @param lens
 * @return short类型
 */
static ushort sumCheck2(const char* arr, int lens) {
    ushort sum = 0;
    for (int i = 0; i < lens; i++) {
        sum += (ushort)(*(arr + i) & 0xff);
    }
    return sum;
}

/**
 * @brief CRC16校验
 * @param data
 * @param length
 * @return
 */
static uint16_t crc16(uchar *data, uchar length) {
    int i;
    uint16_t crc_result = 0xffff;
    while (length--) {
        crc_result ^= (*data++) & 0xff;
        for (i = 0; i < 8; i++) {
            if (crc_result & 0x01)
                crc_result = (crc_result >> 1) ^ 0xa001;
            else
                crc_result = crc_result >> 1;
        }
    }
    return crc_result;
}

ProtocolFlagDataVerify::ProtocolFlagDataVerify(ProtocolFlagDataVerify::VerifyType verifyType)
    : ProtocolFlagData(ProtocolFlag::Flag_Verify)
    , verifyType(verifyType)
{
    switch (verifyType) {
        case Crc16:
            byteSize = 2;
            break;
        case Sum8:
            byteSize = 1;
            break;
        case Sum16:
            byteSize = 2;
            break;
    }
}

QString ProtocolFlagDataVerify::dataToString() {
    switch (verifyType) {
        case Crc16: return "CRC16";
        case Sum8: return "SUM8";
        case Sum16: return "SUM16";
    }
    return {};
}

bool ProtocolFlagDataVerify::verify(char *data, int offset, int maxSize) {
    int verifyBegin = offset;
    int verifySize = 0;
    char* verifyCodePtr = nullptr;
    for (const auto& flag : verifyFlags) {
        switch (flag->flag) {
            case ProtocolFlag::Flag_Header:{
                auto headerFlag = qSharedPointerCast<ProtocolFlagDataHeader>(flagReader->readFlag(flag->flag));
                verifyBegin -= headerFlag->target.size();
                verifySize += headerFlag->target.size();
            }
                break;
            case ProtocolFlag::Flag_Size: {
                auto sizeFlag = qSharedPointerCast<ProtocolFlagDataSize>(flagReader->readFlag(flag->flag));
                verifyBegin -= sizeFlag->byteSize;
                verifySize += sizeFlag->byteSize + sizeFlag->dataSize;
                verifyCodePtr = data + offset + sizeFlag->dataSize;
            }
                break;
            default:
                break;
        }
    }

    if (verifyBegin < 0 || verifySize <= 0) {
        return false;
    }
    if (verifyCodePtr == nullptr) {
        return false;
    }
    switch (verifyType) {
        case Crc16: {
            uint16_t curVerify = crc16((uchar*)(data + verifyBegin), verifySize);
            uint16_t verifyCode = (verifyCodePtr[0] & 0xff) | ((verifyCodePtr[1] & 0xff) << 8);
            return curVerify == verifyCode;
        }
        case Sum8: {
            uchar curVerify = sumCheck(data + verifyBegin, verifySize);
            return curVerify == (uchar)verifyCodePtr[0];
        }
        case Sum16: {
            ushort curVerify = sumCheck2(data + verifyBegin, verifySize);
            auto verifyCode = (ushort)((verifyCodePtr[0] & 0xff) | ((verifyCodePtr[1] & 0xff) << 8));
            return curVerify == verifyCode;
        }
    }
    return false;
}

void ProtocolFlagDataVerify::doFrameOffset(int &offset) {
}

void ProtocolFlagDataVerify::setVerifyFlags(const QList<QSharedPointer<ProtocolFlagData>> &flags) {
    verifyFlags = flags;
}

QByteArray ProtocolFlagDataVerify::getVerifyCode(const QByteArray &buff, int contentOffset) {
    auto dataPtr = buff.data();
    int verifyBegin = contentOffset;
    int verifySize = 0;
    for (const auto& flag : verifyFlags) {
        switch (flag->flag) {
            case ProtocolFlag::Flag_Header:{
                auto headerFlag = qSharedPointerCast<ProtocolFlagDataHeader>(flagReader->readFlag(flag->flag));
                verifyBegin -= headerFlag->target.size();
                verifySize += headerFlag->target.size();
            }
                break;
            case ProtocolFlag::Flag_Size: {
                auto sizeFlag = qSharedPointerCast<ProtocolFlagDataSize>(flagReader->readFlag(flag->flag));
                verifyBegin -= sizeFlag->byteSize;
                verifySize += sizeFlag->byteSize + sizeFlag->dataSize;
            }
                break;
            default:
                break;
        }
    }

    switch (verifyType) {
        case Crc16: {
            uint16_t curVerify = crc16((uchar*)(dataPtr + verifyBegin), verifySize);
            return QByteArray((char*)&curVerify, 2);
        }
        case Sum8: {
            uchar curVerify = sumCheck(dataPtr + verifyBegin, verifySize);
            return QByteArray(1, curVerify);
        }
        case Sum16: {
            ushort curVerify = sumCheck2(dataPtr + verifyBegin, verifySize);
            return QByteArray((char*)&curVerify, 2);
        }
    }
    return QByteArray();
}

QSharedPointer<ProtocolFlagData> ProtocolFlagDataVerify::copy() const {
    return QSharedPointer<ProtocolFlagDataVerify>::create(verifyType);
}

PROTOCOL_CODEC_NAMESPACE_END