#pragma once

#include "taskentity.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include <chrono>
#include <string>

#define TASK_CHECK_STOP_INTERVAL 10

class TaskBase
{
public:
    TaskBase();

    ~TaskBase();

    template<typename Func, typename...Args>
    void addTask(Func&& func, Args&&...args) {
        std::lock_guard<std::mutex> lk(mt);
        auto task = new TaskEntity(std::forward<Func>(func), std::forward<Args>(args)...);
        taskQueue.push(task);
        cv.notify_one();
    }

    void run(std::future<void>&& future);

private:
    std::mutex mt;
    std::condition_variable cv;
    std::queue<TaskEntityBase*> taskQueue;
    std::thread taskThr;
    std::future<void> stopFuture;
    std::promise<void> stopPromise;
};