#pragma once

#include "src/utils/utils.h"
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class DataCache : public Utils::SingleInstance<DataCache<T>>{
public:
    DataCache() = default;

    ~DataCache() = default;

    void addData(T&& data) {
        {
            std::lock_guard<std::mutex> lk(cacheMt);
            dataQueue.push(std::move(data));
        }
        cacheCond.notify_one();
    }

    T fetchData() {
        std::unique_lock<std::mutex> lk(cacheMt);
        while(dataQueue.empty()) {
            cacheCond.wait(lk);
        }
        auto data = dataQueue.front();
        dataQueue.pop();
        return data;
    }

    void clear() {
        std::lock_guard<std::mutex> lk(cacheMt);
        dataQueue.swap(std::queue<T>{});
    }
private:
    std::mutex cacheMt;
    std::condition_variable cacheCond;
    std::queue<T> dataQueue;
};

