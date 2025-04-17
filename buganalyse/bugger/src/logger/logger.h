#pragma once

#include "src/utils/macros.h"
#include "src/utils/utils.h"
#include <mutex>
#include <thread>
#include <iostream>
#include <sstream>

class Logger : public Utils::SingleInstance<Logger> {
public:
    template<typename...Args>
    void log(LogLevel logLevel, const std::string& file, int line, Args&&...args) {
        std::lock_guard<std::mutex> lk(consoleMt);
        buf.clear();
        buf.str("");
        std::string level = "[" + getLogLevelName(logLevel) + "]:";
        level.resize(8, ' ');
        buf << level << " " <<  Utils::curTime() << " " << file << ":" << line << " " << "Thread Id:" <<
                std::this_thread::get_id() << " ";
        printHelp(std::forward<Args>(args)...);
    }

private:
    void printHelp() {
        (std::cout << buf.str() << std::endl).flush();
    }

    template<typename T, typename...Args>
    void printHelp(T&& t, Args&&... args) {
        buf << t << "   ";
        printHelp(std::forward<Args>(args)...);
    }

    std::string getLogLevelName(LogLevel logLevel) {
        auto level = LOG_NAME[logLevel];
        return level.substr(6, level.size());
    }

private:
    std::mutex consoleMt;
    std::stringstream buf;
};

#define LOG_INFO(...) (Logger::instance()->log(LogLevel::LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_WARNING(...) (Logger::instance()->log(LogLevel::LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_ERROR(...) (Logger::instance()->log(LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_FATAL(...) (Logger::instance()->log(LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__))