#include "sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
        : QObject(parent)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorOccurred("WSAStartup Failed!");
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
        WSACleanup();
    }
}

