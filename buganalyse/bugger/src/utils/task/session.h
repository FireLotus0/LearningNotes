#pragma once

#include "taskentity.h"
#include "taskexecutor.h"
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
    Session(SessionType sessionType, const std::string& user, const std::string& passwd, const std::string& ip, unsigned short sshPort = 22, const std::string& id ="");

    virtual ~Session();

    bool isRunning() const;
protected:
    template<typename Caller, typename Func, typename...Args>
    void addTask(TaskType taskType, Caller* caller, Func&& func, Args...args) { // 改成Args&&...args就不行
        if(sessionState == SessionState::INVALID) {
            std::cerr <<__FILE__ << ":" << __LINE__ << "Add Task Failed: Session State Is Invalid" << std::endl;
            return;
        }
        TASK_EXECUTOR.addTask(id, taskType, std::forward<Func>(func), std::move(caller), std::forward<Args>(args)...);
    }


    bool initConnection();

    bool connectToHost();

    bool handshake();

    bool authPasswd();

    bool isSessionValid();

protected:
    SOCKET sock;
    bool sockConnValid;
    std::string ip;
    std::string id;
    unsigned short port;
    std::string user;
    std::string passwd;
    SessionState sessionState;
    SessionType sessionType;
    LIBSSH2_SESSION *session{};
};
