#pragma once

#include "taskentity.h"
#include "utils/utils.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include <chrono>
#include <string>
#include <iostream>
#include <qobject.h>
#include <qdebug.h>

#define TASK_CHECK_STOP_INTERVAL 10

class TaskExecutor {
public:
    static TaskExecutor &getInstance();

    template<typename Func, typename...Args>
    void addTask(unsigned int sessionId, TaskType taskType, Func &&func, Args &&...args) {
        std::lock_guard<std::mutex> lk(taskMt);
        auto iter = sessionTasks.find(sessionId);
        if(iter == sessionTasks.end()) {
            sessionTasks[sessionId] = std::make_pair(INT64_MAX, std::queue<TaskEntityBase*>{});
        }
        qDebug() << sessionTasks[sessionId].first;
        auto task = new TaskEntity(taskType, std::forward<Func>(func), std::forward<Args>(args)...);
        sessionTasks[sessionId].second.push(task);
        cv.notify_one();
    }

    void removeTask(unsigned int id);

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
    bool running;
    unsigned threadCount;
    std::mutex taskMt;
    std::condition_variable cv;
    std::shared_future<void> stopFuture;
    std::promise<void> stopPromise;
    std::vector<std::thread> threads;
    std::hash<std::thread::id> thrIdHasher;
    /// MAP: SessionId--> {HashCode(ThreadId), task queue}
    std::unordered_map<unsigned int, std::pair<std::size_t, std::queue<TaskEntityBase *>>> sessionTasks;
private:
    inline static std::once_flag initFlag;
    inline static TaskExecutor* instance{nullptr};
};

#define TASK_EXECUTOR (TaskExecutor::getInstance())