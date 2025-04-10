#include "taskexecutor.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libssh2.h>

//#define TEST

TaskExecutor::TaskExecutor()
{
    running = false;
    threadCount = std::thread::hardware_concurrency() - 1;
    stopFuture = stopPromise.get_future().share();

    // 初始化 Windows Sockets
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << Utils::curTime() << "WSAStartup failed" << std::endl;
        return;
    }
    if(libssh2_init(0) != 0){
        std::cerr << Utils::curTime() <<  "WSAStartup failed" << std::endl;
    }
}

TaskExecutor::~TaskExecutor() {
    for(auto& thr : threads) {
        if(thr.joinable()) {
            thr.join();
        }
    }
    libssh2_exit();
    WSACleanup();
}

void TaskExecutor::run(std::shared_future<void> future) {
//    auto f = future;

#ifdef TEST
    while(true){};
#else

    while(future.wait_for(std::chrono::milliseconds(TASK_CHECK_STOP_INTERVAL)) == std::future_status::timeout) {
        std::unique_lock<std::mutex> lk(mt);
        TaskEntityBase* taskPtr{nullptr};
        auto thrId = std::this_thread::get_id();
        for(auto& task : tasks) {
            if(task.second.empty()) {
                continue;
            } else {
                taskPtr = task.second.front();
                task.second.pop();
                break;
            }
        }
        if(taskPtr == nullptr) {
            sessionMapThr.erase(thrId);
//            cv.wait(lk, [&]{ return !tasks.empty(); });
        } else {
            std::cout << "Thread: " << thrId << "----" << taskPtr->getTaskName() << std::endl;

            auto idIter = sessionMapThr.find(thrId);
            if(idIter == sessionMapThr.end()) {
                sessionMapThr[thrId] = taskPtr->getSessionPtr();
            }
            if(sessionMapThr[thrId] == taskPtr->getSessionPtr()){
                auto result = taskPtr->execute();
                std::cout << taskPtr->taskResultStr(result) << std::endl;
                delete taskPtr;
            }
        }
    }
#endif
}

TaskExecutor &TaskExecutor::getInstance() {
    std::call_once(initFlag, [&]{
        instance = new TaskExecutor;
    });
    return *instance;
}

bool TaskExecutor::isRunning() const {
    return running;
}

void TaskExecutor::start(int thrCount) {
    threadCount = thrCount;
    for(int i = 0; i < thrCount; i++) {
        threads.emplace_back(std::move(std::thread(&TaskExecutor::run, (TaskExecutor*)this, stopFuture)));
    }
}

void TaskExecutor::stop() {
    stopPromise.set_value();
}

void TaskExecutor::printTasks() {
    std::lock_guard<std::mutex> lk(mt);
    std::stringstream ss;
    for(auto& task : tasks) {
        ss << task.first;
//        for(auto& t : task.second) {
//
//        }
    }
}
