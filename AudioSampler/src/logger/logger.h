#pragma once

#include "src/utils/utils.h"
#include <mutex>
#include <thread>
#include <iostream>

class Logger : public Utils::SingleInstance<Logger> {
public:
    template<typename...Args>
    void log(LogType logType, const std::string& file, int line, Args&&...args) {
        std::lock_guard<std::mutex> lk(consoleMt);
        buf.clear();
        buf << LOG_NAME[logType] <<  Utils::curTime() << "   " << file << ":" << line << "   " <<
                std::this_thread::get_id() << "        ";
        printHelp(std::forward<Args>(args)...);
    }

private:
    void printHelp() {
        (std::cout << buf.str()).flush();
    }

    template<typename T, typename...Args>
    void printHelp(T&& t, Args&&... args) {
        buf << t << "   ";
        printHelp(std::forward<Args>(args)...);
    }

private:
    std::mutex consoleMt;
    std::stringstream buf;

};

#define INFO(...) (Logger::instance()->log(LogType::INFO, __FILE__, __LINE__, __VA_ARGS__))
#define WARNING(...) (Logger::instance()->log(LogType::WARNING, __FILE__, __LINE__, __VA_ARGS__))
#define ERROR(...) (Logger::instance()->log(LogType::ERROR, __FILE__, __LINE__, __VA_ARGS__))
#define FATAL(...) (Logger::instance()->log(LogType::FATAL, __FILE__, __LINE__, __VA_ARGS__))