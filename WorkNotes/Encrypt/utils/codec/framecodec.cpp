

#include "framecodec.h"

void FrameCodec::appendBuffer(const std::vector<uint8_t> &data) {
    bool isFirstFrame = false, finished = false;
    if(curType == INT16_MAX) {
        // first frame
        buffer.append(data);
        if(buffer.fetch(0, header.length()) != header) {
            LOG_WARNING("Frame Header Mismatch: Receive=", buffer.toHexStr(true), "Header=", header.toHexStr(true),
                        "all data:", buffer.toHexStr(true));
            buffer.clear();
            return;
        }
        isFirstFrame = true;
    }
    for(int i = 0; i < data.size(); i++) {
        if(data[i] == tail.toInteger<1>()) {
            if(isFirstFrame) {

            }
        }
    }

    if(buffer.length() >= curSize + minSize) {
        bool success{true};
        auto verifyFlag = buffer.fetch(header.length() + size.length() + curSize, verify.length());
        if(verifyFlag != verify) {
            LOG_FATAL("Verify Failed: verify=", verify.toHexStr(), " receive=", verifyFlag.toHexStr(), " all data:",
                      buffer.toHexStr(true));
            success = false;
        }
        auto tailFlag = buffer.fetch(header.length() + size.length() + curSize + tail.length(), tail.length());
        if(tailFlag != tail) {
            LOG_FATAL("Tail Mismatch: tail=", tail.toHexStr(), " receive=", tailFlag.toHexStr(), " all data:",
                      buffer.toHexStr(true));
            success = false;
        }
        if(!success) {
            curSize = 0;
            buffer.clear();
            return;
        } else {
            LOG_INFO("Decode Finish:", buffer.toHexStr(true));

            curSize = 0;
        }
    }
}

void FrameCodec::setFrameFormat(const std::string &format) {

}

FrameCodec::FrameCodec() {
    header = ByteArray::fromHex("AAFF");
    tail = ByteArray::fromHex("AF");
    minSize = 6;
    dataStartPos = 5;
}


