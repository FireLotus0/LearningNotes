#pragma once

#include "taskentity.h"
#include "utils.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include <chrono>
#include <string>
#include <iostream>

#define TASK_CHECK_STOP_INTERVAL 10

class TaskExecutor {
public:
    static TaskExecutor &getInstance();

    template<typename Func, typename...Args>
    void addTask(const std::string& sessionId, TaskType taskType, Func &&func, Args &&...args) {
        std::lock_guard<std::mutex> lk(mt);
        auto iter = tasks.find(sessionId);
        if(iter == tasks.end()) {
            tasks[sessionId] = std::queue<TaskEntityBase*>{};
        }
        auto task = new TaskEntity(taskType, std::forward<Func>(func), std::forward<Args>(args)...);
        tasks[sessionId].push(task);
        cv.notify_one();
    }

    void run(std::shared_future<void> future);

    bool isRunning() const;

    void start(int thrCount = std::thread::hardware_concurrency() - 1);

    void stop();

    void printTasks();
private:
    TaskExecutor();

    ~TaskExecutor();

    TaskExecutor(const TaskExecutor &) = delete;

    TaskExecutor(TaskExecutor &&) = delete;

    TaskExecutor &operator=(const TaskExecutor &) = delete;

    TaskExecutor &operator=(TaskExecutor &&) = delete;

private:
    std::mutex mt;
    bool running;
    unsigned threadCount;
    std::condition_variable cv;
    std::unordered_map<std::string, std::queue<TaskEntityBase *>> tasks;
    std::unordered_map<std::thread::id, Session*> sessionMapThr;
    std::shared_future<void> stopFuture;
    std::promise<void> stopPromise;
    std::vector<std::thread> threads;

private:
    inline static std::once_flag initFlag;
    inline static TaskExecutor* instance{nullptr};
};

#define TASK_EXECUTOR (TaskExecutor::getInstance())