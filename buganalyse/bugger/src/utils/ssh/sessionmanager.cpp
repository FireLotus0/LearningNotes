#include "sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
        : QObject(parent)
{
    WSADATA wsaData;
    // 初始化 libssh2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorOccurred("WSAStartup Failed!");
    } else if(libssh2_init(0) != 0){
        errorOccurred("Lib Ssh Init Failed!");
    } else {
        wsValid = true;
    }
}

SessionManager &SessionManager::getInstance() {
    static SessionManager sessionManager;
    return sessionManager;
}

SessionManager::~SessionManager() {
    if(wsValid) {
        libssh2_exit();
        WSACleanup();
    }
}

void SessionManager::createSession(QString ip, QString passwd, QString user) {
    if(wsValid) {
        auto session = new Session(ip, passwd, user);
    }
}

