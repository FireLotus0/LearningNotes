#include "utils.h"

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

}