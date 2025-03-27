#pragma once

#include <qobject.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <cstdlib>
#include <QQueue>
#include "taskexecutor.h"

//class SshTask;

/*
void sftp_example(const char* hostname, const char* username, const char* password) {
    int rc;
    // 创建 SSH 会话
    session = libssh2_session_init();
    if (!session) {
        std::cerr << "Failed to create SSH session" << std::endl;
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    // 进行 SSH 握手
    if (libssh2_session_handshake(session, sock)) {
        std::cerr << "Failure establishing SSH session" << std::endl;
        libssh2_session_free(session);
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    // 用户认证
    if (libssh2_userauth_password(session, username, password)) {
        std::cerr << "Authentication by password failed" << std::endl;
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    // 启动 SFTP 会话
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        std::cerr << "Unable to initialize SFTP session" << std::endl;
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    // 上传文件
    sftp_handle = libssh2_sftp_open(sftp_session, "remote_file.txt",
                                    LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
                                    LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR);
    if (!sftp_handle) {
        std::cerr << "Unable to open file for writing" << std::endl;
        libssh2_sftp_shutdown(sftp_session);
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    const char *data_to_send = "Hello, SFTP!";
    rc = libssh2_sftp_write(sftp_handle, data_to_send, strlen(data_to_send));
    if (rc < 0) {
        std::cerr << "Error writing file: " << rc << std::endl;
    }

    libssh2_sftp_close_handle(sftp_handle);

    // 下载文件
    sftp_handle = libssh2_sftp_open(sftp_session, "remote_file.txt", LIBSSH2_FXF_READ, 0);
    if (!sftp_handle) {
        std::cerr << "Unable to open file for reading" << std::endl;
        libssh2_sftp_shutdown(sftp_session);
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
        closesocket(sock);
        libssh2_exit();
        WSACleanup();
        return;
    }

    memlen = 1024; // 读取的字节数
    mem = (char *)malloc(memlen);
    rc = libssh2_sftp_read(sftp_handle, mem, memlen);
    if (rc < 0) {
        std::cerr << "Error reading file: " << rc << std::endl;
    } else {
        std::cout << "File contents: " << std::string(mem, rc) << std::endl;
    }

    free(mem);
    libssh2_sftp_close_handle(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);
    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    closesocket(sock);
    WSACleanup();
}

 * */

class Session : public QObject {
Q_OBJECT
public:
    explicit Session(QString ip, QString passwd, QString user, QObject *parent = nullptr);

    ~Session();

public slots:

signals:
    void errorOccurred(const QString &error);

private:
    QString ip, passwd, user;
    LIBSSH2_SFTP *sftpSession{nullptr};
    LIBSSH2_SFTP_HANDLE *sftpHandle{nullptr};
    LIBSSH2_CHANNEL *channel{nullptr};
    SOCKET sock = INVALID_SOCKET;
    LIBSSH2_SESSION *sshSession{nullptr};
    friend class TaskExecutor;
    TaskExecutor* taskExecutor;
};
