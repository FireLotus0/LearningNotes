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
        auto result = (*task)();
        std::cout << task->taskResultStr(result) << std::endl;
    }
}

std::string TaskBase::getCurTime() {
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    char timeBuf[40]{};
    std::strftime(timeBuf, 40, "%Y-%m-%d %H:%M:%S", std::localtime(&now_t));
    std::string res(timeBuf);

    std::stringstream ss;
    ss << timeBuf << "." << std::setw(3) << std::setfill('0') << std::to_string(milliseconds.count()) << " ";
    return ss.str();
}
