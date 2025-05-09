#pragma once

#include "session.h"
#include <sstream>


class ChannelSession : public Session
{
public:
    ChannelSession(const std::string& user, const std::string& passwd, const std::string& ip, const std::string& sessionName, unsigned int id, unsigned short sshPort = 22);

    ~ChannelSession() override;

    void addCmdTask(const std::string& cmd);

    void executeCallback(TaskType taskType, unsigned long taskId, bool succeed) override;

    SessionType sessionType() const override;

    void addCreateTask() override;
private:
    bool initChannel();

    bool runCommand(const std::string& command);

    void releaseChannel();
private:
    LIBSSH2_CHANNEL* channel;
    bool channelValid;
    std::stringstream resBuf;
};