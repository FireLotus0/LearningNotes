#include "bytearray.h"
#include "utils/logger/logger.h"
#include <cassert>
#include <iostream>

ByteArray::ByteArray(std::size_t size) {
    buffer.resize(size);
}

ByteArray::ByteArray(const ByteArray &other) {
    buffer = other.buffer;
}

ByteArray::ByteArray(const std::vector<std::byte> &data) {
    buffer = data;
}

ByteArray::ByteArray(std::vector<std::byte> &&data) {
    buffer = std::move(data);
}

ByteArray::ByteArray(ByteArray &&other)  noexcept {
    buffer = std::move(other.buffer);
}

ByteArray &ByteArray::operator=(const ByteArray &other) {
    if(&other != this) {
        buffer = other.buffer;
    }
    return *this;
}

ByteArray &ByteArray::operator=(ByteArray &&other) noexcept {
    if(&other != this) {
        buffer = std::move(other.buffer);
    }
    return *this;
}

ByteArray::ByteArray(const std::string &str) {
    auto sz = str.length();
    buffer.resize(sz);
    for(int i = 0; i < sz; i++) {
        buffer[i] = static_cast<std::byte>(str[i]);
    }
}

ByteArray::ByteArray(const char *str, std::size_t length) {
    buffer.resize(length);
    for(int i = 0; i < length; i++) {
        buffer[i] = static_cast<std::byte>(str[i]);
    }
}

std::byte &ByteArray::operator[](std::size_t index) {
    assert(index < buffer.size());
    return buffer[index];
}

ByteArray::iter ByteArray::begin() {
    return buffer.begin();
}

ByteArray::iter ByteArray::end() {
    return buffer.end();
}

ByteArray ByteArray::fromHex(const std::string &hexStr) {
    ByteArray res;
    auto sz = hexStr.length();
    res.resize((sz - 1) / 2 + 1);
    int i = 0;
    for(; i < sz; i+= 2) {
        if(i == 0 && sz >= 2 && tolower(hexStr[i + 1]) == 'x' && hexStr[i] == '0') {
            continue;
        } else {
            res[i / 2] = static_cast<std::byte>((charToHex(hexStr[i]) << 4) + charToHex(hexStr[i + 1]));
        }
    }
    if(i < sz) {
        res[sz - 1] = static_cast<std::byte>(charToHex(hexStr[i]) << 4);
    }
    return res;
}

void ByteArray::resize(std::size_t sz) {
    buffer.resize(sz);
}

std::string ByteArray::toHexStr(bool seperated) const {
    std::string res;
    auto step = seperated ? 3 : 2;
    auto bufferSz = buffer.size();
    auto sz = bufferSz * step;
    res.resize(sz);
    for(int i = 0; i < bufferSz; i++) {
        res[i * step] = hexToChar((uint8_t )buffer[i] >> 4);
        res[i * step + 1] = hexToChar((uint8_t )buffer[i] & 0xF);
        if(seperated) {
            res[i * step + 2] = ' ';
        }
    }
    return res;
}

std::size_t ByteArray::length() const {
    return buffer.size();
}

std::string ByteArray::toString() const {
    std::string res;
    auto sz = buffer.size();
    res.resize(sz);
    for(int i = 0; i < sz; i++) {
        res[i] = static_cast<char>(buffer[i]);
    }
    return res;
}

uint8_t ByteArray::charToHex(char c) {
    auto ascVal = uint8_t(c);
    if(ascVal >= 48 && ascVal <= 57) {
        return ascVal & 0xf;
    } else if(ascVal >= 65 && ascVal <= 70) {
        return ascVal - 55;
    } else if(ascVal >= 97 && ascVal <= 102) {
        return ascVal - 87;
    } else {
        return 0;
    }
}

char ByteArray::hexToChar(uint8_t val) {
    if(val >= 0 && val <= 9) {
        return char(0x30 + val);
    } else if(val >= 10 && val <= 15) {
        return char(val - 10 + 0x41);
    } else {
        return '0';
    }
}

std::size_t ByteArray::insert(const ByteArray &data, std::size_t pos) {
    auto dataSz = data.length();
    if(pos + dataSz > buffer.size()) {
        LOG_FATAL("Insert Out Of Range: pos=", pos, "buf size=", buffer.size());
        return 0;
    }
    auto index = pos, endIndex = pos + dataSz;
    for(; index < endIndex; index++) {
        buffer[index] = data[index - pos];
    }
    return endIndex;
}

const std::byte &ByteArray::operator[](std::size_t index) const {
    assert(index < buffer.size());
    return buffer[index];
}

bool ByteArray::insert(const ByteArray &h, const ByteArray &s, const ByteArray &c, const ByteArray &v, const ByteArray &e) {
    auto insertSz = h.length() + s.length() + c.length() + v.length() + e.length();
    if(insertSz > buffer.size()) {
        LOG_FATAL("Insert Out Of Range: insertSz=", insertSz, " bufferSz:", buffer.size());
        buffer.clear();
        return false;
    }
    auto index = insert(h, 0);
    index = insert(s, index);
    index = insert(c, index);
    index = insert(c, index);
    index = insert(e, index);
    return true;
}

ByteArray ByteArray::fromIntArray(const std::vector<uint8_t> &data) {
    auto sz = data.size();
    ByteArray res(sz);
    for(int i = 0; i < sz; i++) {
        res[i] = std::byte(data[i]);
    }
    return res;
}

std::vector<uint8_t> ByteArray::toIntArray() const {
    auto sz = buffer.size();
    std::vector<uint8_t> res(sz);
    for(int i = 0; i < sz; i++) {
        res[i] = static_cast<uint8_t>(buffer[i]);
    }
    return res;
}

void ByteArray::append(const ByteArray &data) {
    auto sz = buffer.size();
    buffer.resize(data.length() +length());
    insert(data, sz);
}

void ByteArray::append(const std::vector<uint8_t> &data) {
    append(ByteArray::fromIntArray(data));
}

bool ByteArray::operator==(const ByteArray &other) const {
    return other.buffer == buffer;
}

ByteArray ByteArray::fetch(std::size_t start, std::size_t length) const {
    /// TODO: Range Check
    return ByteArray(std::vector<std::byte>(buffer.begin() + start, buffer.begin() + start + length));
}

std::vector<std::byte> ByteArray::data() {
    return buffer;
}

bool ByteArray::operator!=(const ByteArray &other) const {
    return !(other == *this);
}

void ByteArray::clear() {
    buffer.clear();
}

ByteArray &ByteArray::operator+=(const ByteArray &other) {
    append(other);
    return *this;
}

ByteArray ByteArray::operator+(const ByteArray &other) {
    auto tmp = *this;
    tmp.append(other);
    return tmp;
}

 std::byte ByteArray::get(std::size_t index) const {
    /// TODO: check range
    return buffer[index];
}

