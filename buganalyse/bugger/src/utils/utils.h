#pragma once

#include <string>
#include <vector>
#include <atomic>

namespace Utils {

std::string curTime();

std::vector<char> readFile(const std::string &filePath);

unsigned long curTimeStamp();

template<typename T>
class SingleInstance {
public:
    template<typename...Args>
    static T *instance(Args &&... args) {
        auto old = singleInstance.load(std::memory_order_acquire);
        if (old != nullptr) {
            return old;
        }
        while (!singleInstance.compare_exchange_weak(old, new T(std::forward<Args>(args)...),
                                                     std::memory_order_acq_rel));
        return singleInstance.load(std::memory_order_acquire);
    }

protected:
    SingleInstance() = default;

    virtual ~SingleInstance() = default;

private:
    SingleInstance(const SingleInstance<T> &) = delete;

    SingleInstance(SingleInstance<T> &&) = delete;

    SingleInstance<T> &operator=(const SingleInstance<T> &) = delete;

    SingleInstance<T> &operator=(SingleInstance<T> &&) = delete;

private:
    inline static std::atomic<T *> singleInstance{nullptr};
};
}