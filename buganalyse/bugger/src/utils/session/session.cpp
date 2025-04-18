#include "session.h"
#include "utils/task/taskexecutor.h"
#include <cassert>

Session::Session(SessionType sessionType, const std::string &user, const std::string &passwd, const std::string &ip, const std::string& sessionName, unsigned int id, unsigned short sshPort)
    : type(sessionType)
    , user(user)
    , passwd(passwd)
    , ip(ip)
    , sessionName(sessionName)
    , id(id)
    , port(sshPort)
{
    sock = INVALID_SOCKET;
    sockConnValid = false;
    sessionState = SessionState::INVALID;
}

Session::~Session() {
    if(session != nullptr) {
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
    }
    if(sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

bool Session::initConnection() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        LOG_ERROR("Socket Create Failed!");
        return false;
    }
    session = libssh2_session_init();
    if(session == nullptr) {
        LOG_ERROR("Session Create Failed!");
        closesocket(sock);
        sock = INVALID_SOCKET;
        return false;
    }

    return connectToHost();
}

bool Session::connectToHost() {
    assert(sock != INVALID_SOCKET);
    sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, ip.c_str(), &sin.sin_addr);
    auto rt = ::connect(sock, (struct sockaddr *) (&sin), sizeof(sin));
    if (rt != 0) {
        LOG_ERROR("Connect Server Failed: IP=", ip, " Port=", port);
        return false;
    }
    sockConnValid = true;
    return handshake();
}

bool Session::handshake() {
    assert(sockConnValid);
    if(libssh2_session_handshake(session, sock) != 0) {
        LOG_ERROR("SSH Handshake Failed!");
        return false;
    }
    return authPasswd();
}

bool Session::authPasswd() {
    if(libssh2_userauth_password(session, user.c_str(), passwd.c_str()) != 0) {
        LOG_ERROR("Auth Passwd Failed: User=", user, "Password=", passwd);
        return false;
    }
    sessionState = SessionState::IDLE;
    return true;
}

bool Session::isSessionValid() {
    return sockConnValid && sessionState != SessionState::INVALID;
}

bool Session::isRunning() const {
    return sessionState == SessionState::RUNNING;
}

void Session::registerCallback(TaskType taskType, QObject *object, const std::string &methodName) {
    auto methodIndex = object->metaObject()->indexOfSlot(methodName.c_str());
    assert(methodIndex != -1);
    auto metaMethod = object->metaObject()->method(methodIndex);
    callbacks[taskType] = std::make_pair(object, metaMethod);
}


bool Session::isTaskTypeRemove(TaskType taskType) {
    if(taskType == TaskType::REMOVE_ALL_TASK) {
        callbacks[taskType].second.invoke(callbacks[taskType].first, Qt::QueuedConnection, Q_ARG(unsigned int, id));
        return true;
    }
    return false;
}

void Session::addCreateTask() {
    TASK_EXECUTOR.addTask(id, TaskType::INIT_CONNECTION, &Session::initConnection, this);
}

std::string Session::getUserIp() const {
    return user + "@" + "ip";
}

std::string Session::getLastError() {
    char *error_msg;
    int error_msg_len;
    libssh2_session_last_error(session, &error_msg, &error_msg_len, 0);
    return std::string(error_msg, error_msg_len);
}
