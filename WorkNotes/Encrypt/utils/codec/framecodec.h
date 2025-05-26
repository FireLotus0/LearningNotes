#pragma oncn
#include "utils/bytearray/bytearray.h"
#include "utils/logger/logger.h"
#include "crc.h"
#include "utils/encrypt/encryptor.h"
#include "utils/json/json.h"
#include <functional>
#include <unordered_map>

class FrameCodec {
public:
    FrameCodec();

    void setFrameFormat(const std::string& format);

    template<unsigned short Type>
    ByteArray encode(const ByteArray& dataContent) {
        auto type = ByteArray::fromData(Type);
        auto content = type + dataContent;
        auto size = ByteArray::fromData<uint8_t>((uint8_t)content.length());
        auto verify = CRC::CRC16(header + size + content);

        ByteArray encodeBuf(minSize + content.length());
        auto tmp = header+size+ content+verify+tail;
        if(encodeBuf.insert(tmp, 0)) {
            LOG_DEBUG("Encode Data:", encodeBuf.toHexStr());
            dataCache[type.toInteger<2>()] = ByteArray{};
        }
        return encodeBuf;
    }

    /*!
     * @note decode a complete frame
     * @tparam T
     * @param completeFrame
     * @return
     */
    template<typename T>
    std::pair<bool, T> decode(const ByteArray& completeFrame) {
        auto res = std::make_pair(false, T{});
        if(completeFrame.fetch(0, 2) != header) {
            LOG_INFO("Decode Error: header mismatch, expected:", header.toHexStr(), "frame:", completeFrame.toHexStr());
            return res;
        }
        auto contentSz = completeFrame.fetch(sizePos, 1).toInteger<1>() - 2;
        auto type = completeFrame.fetch(typePos, 2);
        auto verifyPos = dataPos + contentSz;
        auto receiveVerify = completeFrame.fetch(verifyPos, 2);
        auto receiveTail = completeFrame.fetch(verifyPos + 2, 1);
        if(receiveTail != tail) {
            LOG_INFO("Decode Error: tail mismatch, expected:", tail.toHexStr(), "frame:", completeFrame.toHexStr());
            return res;
        }
        if(CRC::CRC16(completeFrame.fetch(0, verifyPos)) != receiveVerify) {
            LOG_INFO("Decode Error: crc16 mismatch, expected:", "frame:", completeFrame.toHexStr());
            return res;
        }
        auto data = completeFrame.fetch(dataPos, contentSz);
        if(T::Type != 3) {
            try
            {
                auto j = json::parse(Encryptor::decrypt(data.toString()));  // 从字符串反序列化
                LOG_INFO("JSON parse finish", j.dump());
                res.first = true;
                res.second = j.template get<T>();
                return res;
            }
            catch(std::exception& e) // 捕获异常
            {
                LOG_ERROR("Json Parse Failed:", e.what());
            }
        } else {
            res.first = true;
            return res;
        }
    }

    template<unsigned short Key>
    void registerCallback(const std::function<void(const ByteArray&)>& decodeCallback) {
        if(decodeCallbacks.find(Key) != decodeCallbacks.end()) {
            LOG_WARNING("Decode Callback Already Exists: type=", Key);
            return;
        }
        decodeCallbacks[Key] = decodeCallback;
    }

    void appendBuffer(const std::vector<uint8_t>& data);

    void appendBuffer(const ByteArray& data);
private:
    // Type -- {verify, data}
    std::unordered_map<unsigned short, ByteArray> dataCache;
    std::unordered_map<unsigned int, std::function<void(const ByteArray&)>> decodeCallbacks;
    ByteArray header, tail;
    std::size_t minSize{0}, sizePos{2}, typePos{3}, dataPos{5};
    unsigned short curType = INT16_MAX;
};


