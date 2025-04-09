#pragma once

#include <qobject.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
//#include "libssh2.h"
//#include "libssh2_sftp.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <cstdlib>
#include <QQueue>
#include "taskexecutor.h"

class Session : public QObject {
Q_OBJECT
public:
    explicit Session(const QString& ip, const QString& passwd, const QString& user, QObject *parent = nullptr);

    ~Session();

    void executeCommand(const QString& cmd);

private slots:
    void onExecuteError(TaskExecutor::ErrorLevel errorLevel, const QString& error);

    void onTaskFinished(TaskExecutor::TaskType taskType, bool res);

//    void onChannelOpened(LIBSSH2_CHANNEL *openedChannel);
signals:
    void errorOccurred(const QString &error);

private:
    void init();

    void clearResource();

    bool isChannelValid() const;

    bool isSftpValid() const;
private:
    QString ip, passwd, user;
    LIBSSH2_SFTP *sftpSession{nullptr};
    LIBSSH2_SFTP_HANDLE *sftpHandle{nullptr};
    LIBSSH2_CHANNEL *channel = nullptr;
    SOCKET sock = INVALID_SOCKET;
    LIBSSH2_SESSION *sshSession{nullptr};
    friend class TaskExecutor;
    TaskExecutor* taskExecutor;
    bool isStateValid = false;
};
