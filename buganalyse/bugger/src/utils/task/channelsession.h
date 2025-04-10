#pragma once

#include "session.h"
#include <sstream>


class ChannelSession : public Session
{
public:
    ChannelSession(const std::string& user, const std::string& passwd, const std::string& ip, unsigned short sshPort = 22, const std::string& id ="");

    ~ChannelSession() override;

    void addCmdTask(const std::string& cmd);

private:
    bool initChannel();

    bool runCommand(const std::string& command);

private:
    LIBSSH2_CHANNEL* channel;
    bool channelValid;
    std::stringstream resBuf;
};