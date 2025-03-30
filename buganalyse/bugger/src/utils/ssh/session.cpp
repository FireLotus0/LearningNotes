#include "session.h"

#include <utility>


Session::Session(const QString& ip, const QString& passwd, const QString& user, QObject *parent)
    : QObject(parent)
    , ip(ip)
    , user(user)
    , passwd(passwd)
{
    init();
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
        return;
    }
    sshSession = libssh2_session_init();
    if(sshSession == nullptr) {
        errorOccurred("Init ssh session failed!");
        closesocket(sock);
        sock = INVALID_SOCKET;
        return;
    }
    taskExecutor = new TaskExecutor(this);
    connect(taskExecutor, &TaskExecutor::taskFinished, this, &Session::onTaskFinished);
    taskExecutor->addTask<TaskExecutor::INIT_CONNECTION>(sock, ip, sshSession, user, passwd);
}

void Session::clearResource() {
    if(channel) {
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        channel = nullptr;
    }
    if(sshSession != nullptr) {
        libssh2_session_disconnect(sshSession, "Normal Shutdown");
        libssh2_session_free(sshSession);
        sshSession = nullptr;
    }
    if(sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

void Session::onExecuteError(TaskExecutor::ErrorLevel errorLevel, const QString &error) {
    switch((int)errorLevel) {
        case (int)TaskExecutor::NORMAL:
            isStateValid = true;
            break;
        case (int)TaskExecutor::FATAL:
            isStateValid = false;
            clearResource();
            break;
    }
    emit(error);
}

void Session::onTaskFinished(TaskExecutor::TaskType type, bool res) {
    QString taskStr = QMetaEnum::fromType<TaskExecutor::TaskType>().valueToKey(type);
    QString resStr = res ? (taskStr + " Success") : (taskStr + " Failed");
    qDebug() << resStr;
    if(!res) {
        onExecuteError(taskExecutor->getErrLelFromTask(type), resStr);
    } else {
        if(type == TaskExecutor::OPEN_CHANNEL) {
            Q_ASSERT(channel != nullptr);
        }
    }
}

void Session::executeCommand(const QString &cmd) {
    if(isChannelValid()) {
        taskExecutor->addTask<TaskExecutor::EXECUTE_CMD>(channel, cmd.toStdString().c_str());
    } else if(sshSession){
        taskExecutor->addTask<TaskExecutor::OPEN_CHANNEL>(sshSession,&channel);
        taskExecutor->addTask<TaskExecutor::EXECUTE_CMD>(channel, cmd.toStdString().c_str());
    } else {
        errorOccurred("Channel is invalid!");
    }
}

bool Session::isChannelValid() const {
    return isStateValid && channel;
}

bool Session::isSftpValid() const {
    return isStateValid && sftpSession && sftpHandle;
}



