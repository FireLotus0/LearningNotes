#pragma once

#include "utils.h"
#include <mutex>
#include <thread>
#include <iostream>
#include <sstream>
#include <qstring.h>

#pragma once
#include <string>

/*------ Log Level Define ------*/
#define MAKE_LOG_LEVEL(GEN) \
    GEN(LEVEL_DEBUG), \
    GEN(LEVEL_INFO), \
    GEN(LEVEL_WARNING), \
    GEN(LEVEL_ERROR),   \
    GEN(LEVEL_FATAL),

#define LOG_ENUM(x) x
enum LogLevel{
    MAKE_LOG_LEVEL(LOG_ENUM)
};

#define LOG_NAME(x) #x
static std::string LOG_NAME[] = {
        MAKE_LOG_LEVEL(LOG_NAME)
};

#ifndef LOG_LEVEL_VAL
#define LOG_LEVEL_VAL (LogLevel::FATAL + 1)
#endif

class Logger : public Utils::SingleInstance<Logger> {
public:
    template<typename...Args>
    void log(LogLevel logLevel, const std::string& file, int line, Args&&...args) {
        if(logLevel < LOG_LEVEL_VAL) {
            return;
        }
        std::lock_guard<std::mutex> lk(consoleMt);
        buf.clear();
        buf.str("");
        std::string level = "[" + getLogLevelName(logLevel) + "]:";
        level.resize(8, ' ');
        buf << level << " " <<  Utils::curTime() << " " << "Thread:" <<
                std::this_thread::get_id() << " ";
        printHelp(std::forward<Args>(args)...);
        buf << " " << file << ":" << line;
        (std::cout << buf.str() << std::endl).flush();
    }

private:
    template<typename T, typename...Args>
    void printHelp(T&& t, Args&&... args) {
        if constexpr (std::is_same_v<QString, std::decay_t<decltype(t)>>) {
            buf << t.toStdString() << "  ";
        } else {
            buf << t << "  ";
        }
        if constexpr (sizeof...(args) > 0) {
            printHelp(std::forward<Args>(args)...);
        }
    }

    std::string getLogLevelName(LogLevel logLevel) {
        auto level = LOG_NAME[logLevel];
        return level.substr(6, level.size());
    }

private:
    std::mutex consoleMt;
    std::stringstream buf;
};

#define LOG_DEBUG(...) (Logger::instance()->log(LogLevel::LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_INFO(...) (Logger::instance()->log(LogLevel::LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_WARNING(...) (Logger::instance()->log(LogLevel::LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_ERROR(...) (Logger::instance()->log(LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_FATAL(...) (Logger::instance()->log(LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__))