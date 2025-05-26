#include <iostream>
#include <vector>
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "utils/codec/framecodec.h"

struct Test {
    enum {
        Type = 11
    };
    uint8_t data = 9;
};

int main()
{
    Test t;
    FrameCodec codec;
    codec.encode(t);

    codec.appendBuffer(ByteArray::fromHex("AAFF030B0009BD85AF").toIntArray());
    return 0;
}