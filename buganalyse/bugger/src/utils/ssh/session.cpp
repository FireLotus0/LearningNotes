#include "session.h"

#include <utility>


Session::Session(const QString& ip, const QString& passwd, const QString& user, QObject *parent)
    : QObject(parent)
    , ip(ip)
    , user(user)
    , passwd(passwd)
{
    init();
    sshSession = libssh2_session_init();
    Q_ASSERT(sshSession != nullptr);
    taskExecutor = new TaskExecutor(this);
    taskExecutor->addTask<TaskExecutor::SERVER_CONNECT>(sock, ip);
    taskExecutor->addTask<TaskExecutor::SSH_HAND_SHAKE>(sshSession, sock);
    taskExecutor->addTask<TaskExecutor::PASSWD_VERIFY>(sshSession, user, passwd);
}

Session::~Session() {
    if(sshSession != nullptr) {
        libssh2_session_free(sshSession);
    }
    if(sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

void Session::init() {
    // 创建 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);;
    if (sock == INVALID_SOCKET) {
        errorOccurred("Failed to create socket");
    }
}

