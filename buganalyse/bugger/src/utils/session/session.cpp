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
    sessionState = SessionState::IDLE;
    TASK_EXECUTOR.addTask(id, TaskType::INIT_CONNECTION, &Session::initConnection, this);
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
    sessionState = SessionState::RUNNING;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << __FILE__ << ":" << __LINE__ << " Socket Create Failed!" << std::endl;
        return false;
    }
    session = libssh2_session_init();
    if(session == nullptr) {
        std::cerr << __FILE__ << ":" << __LINE__ << " Session Create Failed!" << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " Connect Server Failed: IP=" << ip << " PORT=" << port;
        return false;
    }
    sockConnValid = true;
    return handshake();
}

bool Session::handshake() {
    assert(sockConnValid);
    if(libssh2_session_handshake(session, sock) != 0) {
        std::cerr << __FILE__ << ":" << __LINE__ << " SSH Handshake Failed!" << std::endl;
        return false;
    }
    return authPasswd();
}

bool Session::authPasswd() {
    if(libssh2_userauth_password(session, user.c_str(), passwd.c_str()) != 0) {
        std::cerr << __FILE__ << ":" << __LINE__ << " Auth Passwd Failed: User=" << user << " Passwd=" << passwd << std::endl;
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