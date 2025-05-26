
#include "crc.h"

namespace CRC {
    unsigned short CRC_Check(unsigned char* puchMsg, unsigned short usDataLen)
    {
        unsigned char uchCRCHi = 0xFF;  // high byte of CRC initialized
        unsigned char uchCRCLo = 0xFF;  // low byte of CRC initialized
        unsigned char uIndex;           // will index shorto CRC lookup table

        while (usDataLen--) 			// pass through message buffer
        {
            uIndex = uchCRCLo ^ *puchMsg++; // calculate the CRC
            uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
            uchCRCHi = auchCRCLo[uIndex];
        }
        return (uchCRCHi << 8 | uchCRCLo);
    }

    ByteArray CRC16(const ByteArray& data) {
        unsigned char uchCRCHi = 0xFF;  // high byte of CRC initialized
        unsigned char uchCRCLo = 0xFF;  // low byte of CRC initialized
        unsigned char uIndex;           // will index shorto CRC lookup table
        auto usDataLen = data.length();
        auto intArray = data.toIntArray();
        auto iter = intArray.begin();
        while (usDataLen--) 			// pass through message buffer
        {
            uIndex = uchCRCLo ^ *iter++; // calculate the CRC
            uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
            uchCRCHi = auchCRCLo[uIndex];
        }
        return ByteArray::fromData<unsigned short>(uchCRCHi << 8 | uchCRCLo);
    }
}