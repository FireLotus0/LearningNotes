#include "utils.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace Utils{
std::string curTime() {
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    char timeBuf[40]{0};
    std::strftime(timeBuf, 40, "%Y-%m-%d %H:%M:%S", std::localtime(&now_t));
    std::stringstream ss;
    ss << timeBuf << "." << std::setw(3) << std::setfill('0') << std::to_string(milliseconds.count());
    return ss.str();
}

std::vector<char> readFile(const std::string& filePath) {
    std::fstream inFile(filePath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    std::vector<char> result;
    if(inFile.is_open()) {
         auto size = inFile.tellg();
         inFile.seekg(0, std::ios_base::beg);
         result.resize(size);
         inFile.read(result.data(), size);
         inFile.close();
    }
    return result;
}

unsigned long curTimeStamp() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}

}