#include "sshtask.h"
#include "session.h"
#include <qdebug.h>
#include <QMetaEnum>

SshTask::SshTask(Session *session, TaskType type, QObject *parent)
        : QObject(parent), session(session), taskType(type) {
}

bool SshTask::isTaskBlocked(SshTask::TaskType type) {
    if (type == TaskType::SSH_CONNECT) {
        return false;
    } else {
        return true;
    }
}

SshTask::~SshTask() {

}

void SshTask::printResult(SshTask::TaskType type, bool res) {
    QString taskStr = QMetaEnum::fromType<SshTask::TaskType>().valueToKey(type);
    if (!res) {
        libssh2_session_last_error(session->sshSession, &errBuf, &errBufSz, 1);
    }
    auto resStr = taskStr + (res ? " Success" : QString("Failed: %1").arg(QString(errBuf)));
    if (res) {
        qInfo() << resStr;
        emit taskFinished(type);
    } else {
        qWarning() << resStr;
        emit errorOccurred(resStr);
    }
}

void SshTask::doTask() {
    bool res = false;
    switch (taskType) {
        case SERVER_CONNECT: {
            Q_ASSERT(session->sock == INVALID_SOCKET);
            // 连接到服务器
            executeAsync<int>(&SshTask::connectToServer);
            break;
        }
        case SSH_CONNECT: {
            Q_ASSERT(session->sshSession == nullptr);
            session->sshSession = libssh2_session_init();
            res = (session->sshSession != nullptr);
            printResult(SSH_CONNECT, res);
            if(!res) {
                closesocket(session->sock);
                session->sock = INVALID_SOCKET;
            }
            break;
        }
        case SSH_HAND_SHAKE: {
            executeAsync<int>(&libssh2_session_handshake, session->sshSession, session->sock);
            break;
        }
        case PASSWD_VERIFY: {
            executeAsync<int>(this, &SshTask::authUserPasswd);
            break;
        }
        case PUB_KEY_VERIFY: {
            break;
        }
        case OPEN_CHANNEL: {
            break;
        }
        case EXECUTE_CMD: {
            break;
        }
        case READ_DATA: {
            break;
        }
        case WRITE_DATA: {
            break;
        }
        case CHANNEL_FORWARD: {
            break;
        }
        case SFTP_CREATE: {
            break;
        }
        case SFTP_OPEN_FILE: {
            break;
        }
        case SFTP_READ_FILE: {
            break;
        }
        case SFTP_WRITE_FILE: {
            break;
        }
        case SFTP_CLOSE_FILE: {
            break;
        }
        case SFTP_REMOVE_FILE: {
            break;
        }
        case SFTP_MK_DIR: {
            break;
        }
        case SFTP_OPEN_DIR: {
            break;
        }
        case SFTP_READ_DIR: {
            break;
        }
    }
}

int SshTask::connectToServer() {
    // 创建 socket
    session->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (session->sock == INVALID_SOCKET) {
        errorOccurred("Failed to create socket");
    }
    // 设置服务器地址
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22); // SSH 默认端口
    inet_pton(AF_INET, (const char*)session->ip.data(), &sin.sin_addr); // 将 IP 地址字符串转换为网络字节序
    return ::connect(session->sock, (struct sockaddr*)(&sin), sizeof(sin));
}

int SshTask::authUserPasswd() {
    return  libssh2_userauth_password(session->sshSession, (CPTR)session->user.data(), (CPTR)session->passwd.data());
}

//void SshTask::connectSsh() {
    // SSH握手
//    if (libssh2_session_handshake(session->sshSession, session->sock)) {
//        std::cerr << "Failure establishing SSH session" << std::endl;
//        libssh2_session_free(session);
//        closesocket(sock);
//        libssh2_exit();
//        WSACleanup();
//        return;
//    }
//
//    // 用户认证
//    if (libssh2_userauth_password(session, username, password)) {
//        std::cerr << "Authentication by password failed" << std::endl;
//        libssh2_session_disconnect(session, "Normal Shutdown");
//        libssh2_session_free(session);
//        closesocket(sock);
//        libssh2_exit();
//        WSACleanup();
//        return;
//    }

//}
