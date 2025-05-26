#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "utils/logger/logger.h"

class ByteArray;

template<typename T>
constexpr auto is_auto_bytearray = (!std::is_same_v<std::decay_t<T>, ByteArray> && !std::is_constructible_v<std::string, std::decay_t<T>> &&
    std::is_default_constructible_v<T>);

class ByteArray {
    using iter = std::vector<std::byte>::iterator;
public:
    ByteArray() = default;

    ByteArray(std::size_t size);

    ByteArray(const std::vector<std::byte>& data);

    ByteArray(std::vector<std::byte>&& data);

    ByteArray(const ByteArray& other);

    ByteArray(ByteArray&& other) noexcept;

    ByteArray& operator=(const ByteArray& other);

    ByteArray& operator=(ByteArray&& other) noexcept ;

    explicit ByteArray(const std::string& str);

    ByteArray(const char* str, std::size_t length);

    template<typename T, typename = std::enable_if_t<is_auto_bytearray<T>>>
    static ByteArray fromData(T data) {
        ByteArray res;
        res.resize(sizeof(T));
        for(int i = 0; i < sizeof(T); i++) {
            res[i] = static_cast<std::byte>(uint8_t(*((reinterpret_cast<uint8_t*>(&data)) + i)));
        }
        return res;
    }

    template<typename T, typename = std::enable_if_t<is_auto_bytearray<T>>>
    T toData() {
        assert(sizeof(T) == buffer.size());
        T res{};
        int j = 0;
        for(int i = 0; i < sizeof(T); i++) {
            j++;
            *((reinterpret_cast<uint8_t*>(&res)) + i) = static_cast<uint8_t>(buffer[i]);
        }
        return res;
    }

    template<std::size_t byteNums>
    auto toInteger() {
        if constexpr (byteNums == 1) {
            return toData<uint8_t>();
        } else if constexpr (byteNums == 2) {
            return toData<unsigned short >();
        } else if constexpr (byteNums == 4) {
            return toData<unsigned int>();
        } else if constexpr (byteNums == 8) {
            return toData<unsigned long long>();
        } else {
            LOG_FATAL("Byte Nums Mismatch: byteNums=", byteNums, "buffer size=", buffer.size());
            assert(false);
            return (uint8_t)0;
        }
    }

    std::size_t insert(const ByteArray& data, std::size_t pos);

    bool insert(const ByteArray& h, const ByteArray& s, const ByteArray& c, const ByteArray& v, const ByteArray& e);

    std::byte get(std::size_t index) const;

    std::byte& operator[](std::size_t index);

    const std::byte& operator[](std::size_t index) const;

    bool operator==(const ByteArray& other) const;

    bool operator!=(const ByteArray& other) const;

    ByteArray& operator+=(const ByteArray& other);

    ByteArray operator+(const ByteArray& other);

    ByteArray fetch(std::size_t start, std::size_t end) const;

    std::vector<std::byte> data();

    iter begin();

    iter end();

    void resize(std::size_t sz);

    void append(const std::vector<uint8_t>& data);

    void append(const ByteArray& data);

    void clear();
public:
    static ByteArray fromHex(const std::string& hexStr);
    [[nodiscard]] std::string toHexStr(bool seperated = false) const;

    static ByteArray fromIntArray(const std::vector<uint8_t>& data);
    [[nodiscard]] std::vector<uint8_t> toIntArray() const;

public:
    std::size_t length() const;

    std::string toString() const;

private:
    static uint8_t charToHex(char c);
    static char hexToChar(uint8_t val);
private:
    std::vector<std::byte> buffer;

};


