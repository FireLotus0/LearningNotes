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
    QString resStr = res ?  (taskStr + " Success") : (taskStr + " Failed");
    if (res) {
        qInfo() << resStr;
        emit taskFinished(type);
    } else {
        qWarning() << resStr;
        emit errorOccurred(resStr);
    }

    onTaskFinished(type, res);
}


int TaskExecutor::connectToServer(SOCKET sock, const QString& ip) {
    // 设置服务器地址
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    qDebug() << "connect to: " << ip;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, ip.toStdString().c_str(), &sin.sin_addr);
    auto rt = ::connect(sock, (struct sockaddr*)(&sin), sizeof(sin));
    if(rt != 0) {
        qWarning() << "Socket Connect Error: " << WSAGetLastError() << " socket fd: " <<sock << " target ip: " << QString::fromLatin1(ip.toStdString().c_str(), 10);
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
