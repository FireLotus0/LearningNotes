#pragma once

#include "taskentity.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

class Session{
public:
    Session(SessionType sessionType, const std::string& user, const std::string& passwd, const std::string& ip);

    virtual ~Session();
    template<typename Func, typename...Args>
    void addTask(Func&& func, Args&&...args) {
        std::lock_guard<std::mutex> lk(mt);
        if(sessionState == SessionState::INVALID) {
            std::cerr << "Add Task Failed: SessionType=" << SessionName[sessionType] << "  SessionState=" << SessionStateName[sessionState]
                << std::endl;
            return;
        } else {
            auto task = new TaskEntity<Func, Args...>(std::forward<Func>(func), std::forward<Args>(args)...);
            tasks.push(task);
        }
        if(sessionState == SessionState::IDLE) {
            cv.notify_one();
        }
    }

protected:
    SOCKET socket;
    std::string ip;
    std::string user;
    std::string passwd;
    SessionState sessionState;
    SessionType sessionType;
    LIBSSH2_SESSION *session{};

protected:
    std::mutex mt;
    std::condition_variable cv;
    std::queue<TaskEntityBase*> tasks;
};