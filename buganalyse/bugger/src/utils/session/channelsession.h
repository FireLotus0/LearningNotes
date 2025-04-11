#pragma once

#include "session.h"
#include <sstream>


class ChannelSession : public Session
{
public:
    ChannelSession(const std::string& user, const std::string& passwd, const std::string& ip, const std::string& sessionName, unsigned int id, unsigned short sshPort = 22);

    ~ChannelSession() override;

    void addCmdTask(const std::string& cmd);

    void executeCallback(TaskType taskType) override;

private:
    bool initChannel();

    bool runCommand(const std::string& command);

private:
    LIBSSH2_CHANNEL* channel;
    bool channelValid;
    std::stringstream resBuf;
};