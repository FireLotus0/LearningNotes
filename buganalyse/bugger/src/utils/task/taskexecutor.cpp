#include "taskexecutor.h"
#include <iostream>
#include <iomanip>
#include <sstream>

TaskBase::TaskBase()
{
    stopFuture = stopPromise.get_future();
    taskThr = std::thread(&TaskBase::run, this, std::move(stopFuture));
}

TaskBase::~TaskBase() {
    if(taskThr.joinable()) {
        taskThr.join();
    }
}

void TaskBase::run(std::future<void> &&future) {
    auto f = std::move(future);
    while(f.wait_for(std::chrono::milliseconds(TASK_CHECK_STOP_INTERVAL)) == std::future_status::timeout) {
        std::unique_lock<std::mutex> lk(mt);
        cv.wait(lk, [&]{ return !taskQueue.empty(); });
        auto task = taskQueue.front();
        auto result = task->execute();
        std::cout << task->taskResultStr(result) << std::endl;
    }
}
