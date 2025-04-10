#include "session.h"
#include "taskexecutor.h"
#include <cassert>

Session::Session(SessionType sessionType, const std::string &user, const std::string &passwd, const std::string &ip, unsigned short sshPort, const std::string& sessionId)
    : sessionType(sessionType)
    , user(user)
    , passwd(passwd)
    , ip(ip)
    , port(sshPort)
{
    sock = INVALID_SOCKET;
    sockConnValid = false;
    sessionState = SessionState::IDLE;
    if(sessionId.empty()) {
        id = user + std::string("@") + ip;
    } else {
        id = sessionId;
    }
    TASK_EXECUTOR.addTask(sessionId, TaskType::INIT_CONNECTION, &Session::initConnection, this);
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
