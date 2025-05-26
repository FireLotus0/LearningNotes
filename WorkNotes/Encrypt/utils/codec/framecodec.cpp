

#include "framecodec.h"

void FrameCodec::appendBuffer(const std::vector<uint8_t> &data) {
    bool isFirstFrame = false, finished = false, hasError{false};
    if(curType == INT16_MAX) {
        // first frame
        auto buffer = ByteArray::fromIntArray(data);
        if(buffer.fetch(0, header.length()) != header) {
            LOG_WARNING("Frame Header Mismatch: Receive=", buffer.toHexStr(true), "Header=", header.toHexStr(true),
                        "all data:", buffer.toHexStr(true));
            hasError = true;
        }
        isFirstFrame = true;
        auto type = buffer.fetch(typePos, 2).toInteger<2>();
        if(dataCache.find(type) == dataCache.end()) {
            LOG_ERROR("Can not decode frame with type:", curType, "not registered");
            hasError = true;
            assert(false);
        } else {
            curType = type;
        }
    }
    if(!hasError) {
        auto& buffer = dataCache[curType];
        auto curPos = buffer.length();
        buffer.append(data);
        for(auto i = curPos; i < buffer.length(); i++) {
            if(buffer.fetch(i, 1) == tail) {
                if(buffer.fetch(0, 2) != header) {
                    LOG_WARNING("Frame Header Mismatch: Receive=", buffer.fetch(0, 2).toHexStr(true), "Header=", header.toHexStr(true),
                                "all data:", buffer.toHexStr(true));
                    hasError = true;
                } else {
                    if(buffer.fetch(i - 2, 2) != CRC::CRC16(buffer.fetch(0, i - 2))) {
                        LOG_WARNING("CRC Mismatch: Receive=", buffer.fetch(i - 2, 2).toHexStr(true), "CRC=",  CRC::CRC16(buffer.fetch(0, i - 2)).toHexStr(true),
                                    "all data:", buffer.toHexStr(true), " CRC Part=", buffer.fetch(0, i - 2).toHexStr(true));
                        hasError = true;
                    } else {
                        auto contentSz = buffer.fetch(sizePos, 1).toInteger<1>() - 2;
                        auto data = buffer.fetch(dataPos, contentSz);
                        LOG_INFO("Decode Finish: TYPE=", curType, "Data=", data.toHexStr(true));
                        /// TODO: invoke callback
                        std::invoke(decodeCallbacks[curType], data);
                        dataCache[curType].clear();
                        curType = INT16_MAX;
                        break;
                    }
                }
            }
        }
    }

    if(hasError) {
        dataCache[curType].clear();
        curType = INT16_MAX;
    }
}

void FrameCodec::setFrameFormat(const std::string &format) {

}

FrameCodec::FrameCodec() {
    header = ByteArray::fromHex("AAFF");
    tail = ByteArray::fromHex("AF");
    minSize = 6;
}

void FrameCodec::appendBuffer(const ByteArray &data) {
    appendBuffer(data.toIntArray());
}


