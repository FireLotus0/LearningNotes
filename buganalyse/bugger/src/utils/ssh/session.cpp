#include "session.h"

#include <utility>


Session::Session(QString ip, QString passwd, QString user, QObject *parent)
    : QObject(parent)
    , ip(std::move(ip))
    , user(std::move(user))
    , passwd(std::move(passwd))
{
    addTask(SshTask::SERVER_CONNECT);
}

Session::~Session() {
    if(sshSession != nullptr) {
        libssh2_session_free(sshSession);
    }
    if(sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

void Session::onTaskFinished(SshTask::TaskType taskType) {

}

void Session::addTask(SshTask::TaskType taskType) {
    auto task = new SshTask(this, taskType);
    task->doTask();
}


