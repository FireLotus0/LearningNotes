#include "app/app.h"
#include "utils/ssh/sessionmanager.h"

#include <iostream>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <cstdlib>
#include <qapplication.h>


void sftp_example(const char* hostname, const char* username, const char* password) {
    int rc;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    char *mem;
    size_t memlen;

    // 初始化 Windows Sockets
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    // 创建 socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    // 设置服务器地址
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22); // SSH 默认端口
    inet_pton(AF_INET, hostname, &sin.sin_addr); // 将 IP 地址字符串转换为网络字节序

    // 连接到服务器
    if (connect(sock, (struct sockaddr*)(&sin), sizeof(sin)) != 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // 初始化 libssh2
    if (libssh2_init(0) != 0) {
        std::cerr << "Failed to initialize libssh2" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

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
    libssh2_exit();
    WSACleanup();
}

int main(int argc, char** argv) {
//    const char *hostname = "192.168.1.159"; // 替换为你的服务器地址
//    const char *username = "root"; // 替换为你的用户名
//    const char *password = "Xykj20160315"; // 替换为你的密码
//
//    sftp_example(hostname, username, password);
//    return 0;
    QApplication a(argc, argv);

    App app;
    app.show();

    SessionManager::getInstance().createSession("192.168.1.159", "Xykj20160315", "root");

    a.exec();
}
