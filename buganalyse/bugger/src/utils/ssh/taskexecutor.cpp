#include "taskexecutor.h"
#include "session.h"
#include <qdebug.h>
#include <QMetaEnum>

TaskExecutor::TaskExecutor(QObject *parent)
        : QObject(parent)
{
    errBuf = new char[errBufSz];
    buf = new char[bufferSz];
}

TaskExecutor::~TaskExecutor() {
    delete []errBuf;
    delete []buf;
}

void TaskExecutor::printResult(TaskExecutor::TaskType type, bool res, int errCode) {
    QString taskStr = QMetaEnum::fromType<TaskExecutor::TaskType>().valueToKey(type);
    QString resStr = "";
    if (!res) {
        if(type != SERVER_CONNECT && type != SSH_CONNECT) {
//            libssh2_session_last_error(session->sshSession, &errBuf, &errBufSz, 1);
            resStr = taskStr + " Failed: " + QString(errBuf);
        } else {
            resStr = taskStr  + " Failed: " + QString::number(errCode);
        }
    } else {
        resStr = taskStr + " Success";
    }

    if (res) {
        qInfo() << resStr;
        emit taskFinished(type);
    } else {
        qWarning() << resStr;
        emit errorOccurred(resStr);
    }

    onTaskFinished(type, res);
}


int TaskExecutor::connectToServer(SOCKET* socketPtr, const QString& ip) {
    // 创建 socket
    *socketPtr = socket(AF_INET, SOCK_STREAM, 0);
    if (*socketPtr == INVALID_SOCKET) {
        errorOccurred("Failed to create socket");
    }
    // 设置服务器地址
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    qDebug() << ip;
    sin.sin_port = htons(22); // SSH 默认端口
    auto sss = "192.168.1.159";
    inet_pton(AF_INET, sss, &sin.sin_addr); // 将 IP 地址字符串转换为网络字节序 (const char*)session->ip.data()
    auto rt = ::connect(*socketPtr, (struct sockaddr*)(&sin), sizeof(sin));
    if(rt <= 0) {
        qWarning() << "Socket Connect Error: " << WSAGetLastError();
    }
    return rt;
}


void TaskExecutor::onTaskFinished(TaskExecutor::TaskType type, bool res) {
    taskCache.removeFirst();
    if(!taskCache.isEmpty()) {
        std::invoke(taskCache.head().second);
    }
}

int TaskExecutor::authUserPasswd(LIBSSH2_SESSION *session, const QString &user, const QString &passwd) {
    return  libssh2_userauth_password(session, (const char*)user.data(), (const char*)passwd.data());
}

LIBSSH2_CHANNEL *TaskExecutor::openChannel(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *&channel) {
    channel = libssh2_channel_open_session(session);
    return channel;
}
