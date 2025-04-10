#pragma once

#include "utils/task/taskexecutor.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <queue>
#include <mutex>
#include <iostream>
#include <qmetaobject.h>
#include "utils/macros.h"

class TaskExecutor;

class Session{
public:
    Session(SessionType sessionType, const std::string& user, const std::string& passwd, const std::string& ip, const std::string& sessionName, unsigned int id, unsigned short sshPort = 22);

    virtual ~Session();

    bool isRunning() const;

    void registerCallback(TaskType taskType, QObject* object, const std::string& methodName);

    template<typename Caller, typename Func, typename...Args>
    void addTask(TaskType taskType, Caller* caller, Func&& func, Args...args) {
        if(sessionState == SessionState::INVALID) {
            std::cerr <<__FILE__ << ":" << __LINE__ << "Add Task Failed: Session State Is Invalid" << std::endl;
            return;
        }
        TASK_EXECUTOR.addTask(id, taskType, std::forward<Func>(func), std::move(caller), std::forward<Args>(args)...);
    }

    virtual void executeCallback(TaskType taskType) = 0;

protected:
    bool initConnection();

    bool connectToHost();

    bool handshake();

    bool authPasswd();

    bool isSessionValid();

    bool isTaskTypeRemove(TaskType taskType);

protected:
    SOCKET sock;
    std::string ip;
    unsigned int id;
    std::string user;
    std::string passwd;
    std::string sessionName;
    bool sockConnValid;
    bool isTaskSucceed = false;
    unsigned short port;
    SessionType sessionType;
    SessionState sessionState;
    LIBSSH2_SESSION *session{};
    std::unordered_map<TaskType, std::pair<QObject*, QMetaMethod>> callbacks;
};
