#include "taskexecutor.h"
#include "session.h"
#include <qdebug.h>
#include <QMetaEnum>

TaskExecutor::TaskExecutor(QObject *parent)
        : QObject(parent) {
    errBuf = new char[errBufSz];
    buf = new char[bufferSz];
}

TaskExecutor::~TaskExecutor() {
    delete[]errBuf;
    delete[]buf;
}

int TaskExecutor::connectToServer(SOCKET sock, const QString &ip) {
    // 设置服务器地址
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    qDebug() << "connect to: " << ip;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, ip.toStdString().c_str(), &sin.sin_addr);
    auto rt = ::connect(sock, (struct sockaddr *) (&sin), sizeof(sin));
    if (rt != 0) {
        qWarning() << "Socket Connect Error: " << WSAGetLastError() << " socket fd: " << sock << " target ip: "
                   << QString::fromLatin1(ip.toStdString().c_str(), 10);
    }
    return rt;
}


void TaskExecutor::onTaskFinished(TaskExecutor::TaskType type, bool res) {
    if (res) {
        taskCache.removeFirst();
        if (!taskCache.isEmpty()) {
            std::invoke(taskCache.head().second);
        }
    } else if (getErrLelFromTask(type) == ErrorLevel::FATAL) {
        taskCache.clear();
    }
    emit taskFinished(type, res);
}

int TaskExecutor::authUserPasswd(LIBSSH2_SESSION *session, const QString &user, const QString &passwd) {
    return libssh2_userauth_password(session, user.toStdString().c_str(), passwd.toStdString().c_str());
}

LIBSSH2_CHANNEL *TaskExecutor::openChannel(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL **channel) {
    *channel = libssh2_channel_open_session(session);
    if(*channel) {
        libssh2_channel_request_pty(*channel, "xterm"); // 请求一个伪终端
        libssh2_channel_shell(*channel); // 请求一个 shell
    }
    return *channel;
}

TaskExecutor::ErrorLevel TaskExecutor::getErrLelFromTask(TaskExecutor::TaskType taskType) {
    switch (taskType) {
        case SERVER_CONNECT:
        case SSH_HAND_SHAKE:
        case PASSWD_VERIFY:
        case PUB_KEY_VERIFY:
            return TaskExecutor::FATAL;
        default:
            return TaskExecutor::NORMAL;
    }
}

void TaskExecutor::cancelCurTask() {
    Q_ASSERT(!taskCache.isEmpty());
    taskCache.removeFirst();
}

void TaskExecutor::executeCurTask() {
    Q_ASSERT(!taskCache.isEmpty());
    std::invoke(taskCache.first().second);
}

int TaskExecutor::initConnection(SOCKET sock, const QString &ip, LIBSSH2_SESSION *session, const QString &user, const QString &passwd)
{
    if(connectToServer(sock, ip) != 0) {
        taskFinished(SERVER_CONNECT, false);
        return -1;
    } else if(libssh2_session_handshake(session, sock) != 0) {
        taskFinished(SSH_HAND_SHAKE, false);
        return -1;
    } else if(authUserPasswd(session, user, passwd) != 0) {
        taskFinished(PASSWD_VERIFY, false);
        return -1;
    }
    return 0;
}
