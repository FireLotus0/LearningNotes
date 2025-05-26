#pragma once
#include <atomic>

template<typename T>
class SingleInstance {
public:
    template<typename...Args>
    static T* instance(Args&&...args) {
        auto oldVal = instancePtr.load(std::memory_order_acquire);
        if(oldVal == nullptr) {
            while (!instancePtr.compare_exchange_weak(oldVal, new T(std::forward<Args>(args)...), std::memory_order_acq_rel));
        }
        return instancePtr.load(std::memory_order_acquire);
    }
public:
    SingleInstance() = default;
    SingleInstance(const SingleInstance &) = delete;
    SingleInstance(SingleInstance &&) = delete;
    SingleInstance &operator=(const SingleInstance &) = delete;
    SingleInstance &operator=(SingleInstance&&) = delete;

private:
    inline static std::atomic<T*> instancePtr{nullptr};
};
