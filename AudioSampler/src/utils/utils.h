#pragma once

#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <vector>
#include <atomic>


/*------ Task Type Define ------*/
#define MAKE_TASK_TYPE(GEN) \
    GEN(SEND_DATA), \
    GEN(READ_MMAP), \
    GEN(TIMED_TASK)

#define TASK_ENUM(x) x
enum TaskType {
    MAKE_TASK_TYPE(TASK_ENUM)
};

#define TASK_NAME(x) #x
static std::string TASK_NAME[] = {
        MAKE_TASK_TYPE(TASK_NAME)
};

/*------ Log Level Define ------*/
#define MAKE_LOG_TYPE(GEN) \
    GEN(INFO), \
    GEN(WARNING), \
    GEN(ERROR),   \
    GEN(FATAL),

#define LOG_ENUM(x) x
enum LogType {
    MAKE_LOG_TYPE(LOG_ENUM)
};

#define LOG_NAME(x) #x
static std::string LOG_NAME[] = {
        MAKE_LOG_TYPE(LOG_NAME)
};

namespace Utils {
    std::string curTime();

    template<typename T>
    class SingleInstance{
    public:
        template<typename...Args>
        static T* instance(Args&&... args) {
            auto old = singleInstance.load(std::memory_order_acquire);
            if(old != nullptr) {
                return old;
            }
            while(!singleInstance.compare_exchange_weak(old, new T(std::forward<Args>(args)...), std::memory_order_acq_rel));
            return singleInstance.load(std::memory_order_acquire);
        }

    protected:
        SingleInstance() = default;
        virtual ~SingleInstance() = default;
    private:
        SingleInstance(const SingleInstance<T>&) = delete;
        SingleInstance(SingleInstance<T>&&) = delete;
        SingleInstance<T>& operator=(const SingleInstance<T>&) = delete;
        SingleInstance<T>& operator=(SingleInstance<T>&&) = delete;

    private:
        inline static std::atomic<T*> singleInstance{nullptr};
    };
}