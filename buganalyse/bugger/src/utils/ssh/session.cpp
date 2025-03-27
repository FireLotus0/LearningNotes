#include "session.h"

#include <utility>


Session::Session(QString ip, QString passwd, QString user, QObject *parent)
    : QObject(parent)
    , ip(std::move(ip))
    , user(std::move(user))
    , passwd(std::move(passwd))
{
    sshSession = libssh2_session_init();
    taskExecutor = new TaskExecutor(this);
    taskExecutor->addTask(TaskExecutor::SERVER_CONNECT, &sock, ip);
//    taskExecutor->addTask(TaskExecutor::SSH_HAND_SHAKE, &sock, ip);
//    taskExecutor->addTask(TaskExecutor::PASSWD_VERIFY, sshSession, user, passwd);
}

Session::~Session() {
    if(sshSession != nullptr) {
        libssh2_session_free(sshSession);
    }
    if(sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

