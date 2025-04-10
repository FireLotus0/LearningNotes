#include "session.h"

Session::Session(SessionType sessionType, const std::string &user, const std::string &passwd, const std::string &ip)
    : sessionType(sessionType)
    , user(user)
    , passwd(passwd)
    , ip(ip)
{
    sessionState = SessionState::IDLE;
}
