#pragma once
#include "utils/utils.h"
#include "taskexecutor.h"

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <libssh2.h>

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
    while(future.wait_for(std::chrono::milliseconds(TASK_CHECK_STOP_INTERVAL)) == std::future_status::timeout) {
        std::unique_lock<std::mutex> lk(taskMt);
        TaskEntityBase* taskPtr{nullptr};
        std::size_t curThrId = thrIdHasher(std::this_thread::get_id()), taskThrId = -1;
        unsigned int sessionId;
        for(auto& sessionTask : sessionTasks) {
            if(sessionTask.second.second.empty()) {
                continue;
            } else {
                taskPtr = sessionTask.second.second.front();
                sessionId = sessionTask.first;
                taskThrId = sessionTask.second.first;
                sessionTask.second.second.pop();
                break;
            }
        }
        if(taskPtr == nullptr) {
            sessionTasks.clear();
            cv.wait(lk, [&]{ return !sessionTasks.empty(); });
        } else {
            if(taskThrId == -1) {   // 第一次获取任务
                sessionTasks[sessionId].first = curThrId;
            } else if(taskThrId == curThrId) {
                // 移除所有任务
                if(taskPtr->taskType == TaskType::REMOVE_ALL_TASK) {
                    sessionTasks.erase(sessionId);
                }
                lk.unlock();
                bool isSucceed = taskPtr->execute();
                std::cout << taskPtr->taskResultStr(isSucceed) << std::endl;
                delete taskPtr;
            }
        }
    }
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
    if(isRunning()) {
        return;
    }
    threadCount = thrCount;
    for(int i = 0; i < thrCount; i++) {
        threads.emplace_back(std::move(std::thread(&TaskExecutor::run, (TaskExecutor*)this, stopFuture)));
    }
}

void TaskExecutor::stop() {
    stopPromise.set_value();
}

void TaskExecutor::printTasks() {
    std::lock_guard<std::mutex> lk(taskMt);
    std::stringstream ss;
    for(auto& task : sessionTasks) {
        ss << task.first;
//        for(auto& t : task.second) {
//
//        }
    }
}

void TaskExecutor::removeTask(unsigned int id) {
    std::lock_guard<std::mutex> lk(taskMt);
    sessionTasks.erase(id);
}
