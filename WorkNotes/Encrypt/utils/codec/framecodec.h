#pragma oncn
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "crc.h"
#include <functional>
#include <unordered_map>

class FrameCodec {
public:
    FrameCodec();

    void setFrameFormat(const std::string& format);

    template<typename T>
    ByteArray encode(const T& data) {
        auto type = ByteArray::fromData((unsigned short)T::Type);
        auto content = type + ByteArray::fromData<T>(data);
        size = ByteArray::fromData<uint8_t>((uint8_t)content.length());
        verify = CRC::CRC16(header + size + content);

        ByteArray encodeBuf(minSize + content.length());
        auto tmp = header+size+ content+verify+tail;
        if(encodeBuf.insert(tmp, 0)) {
            LOG_DEBUG("Encode Data:", encodeBuf.toHexStr());
        }
        return encodeBuf;
    }

    template<typename T>
    void registerCallback(const std::function<void(const T&)>& decodeCallback) {
        auto key = (unsigned int)T::Type;
        if(decodeCallbacks.find(key) != decodeCallbacks.end()) {
            LOG_WARNING("Decode Callback Already Exists: type=", key);
            return;
        }
        decodeCallbacks[key] = decodeCallback;
    }

    void appendBuffer(const std::vector<uint8_t>& data);

private:
    ByteArray buffer;
    std::unordered_map<unsigned int, std::function<void()>> decodeCallbacks;
    ByteArray header, size, verify, tail;
    std::size_t minSize{0}, dataStartPos;
    unsigned short curType = INT16_MAX;
};


