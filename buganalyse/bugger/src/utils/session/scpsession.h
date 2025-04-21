#pragma once

#include "session.h"

class ScpSession : public Session
{
public:
    ScpSession(const std::string& user, const std::string& passwd, const std::string& ip, const std::string& sessionName, unsigned int id, unsigned short sshPort = 22);

    ~ScpSession() override;

    void addScpTask(const std::string& remoteFile, const std::string& localFile, bool isUpload);

    void executeCallback(TaskType taskType, unsigned long taskId, bool succeed) override;

    SessionType sessionType() const override;

    void addCreateTask() override;
private:
    bool uploadFile(const std::string& remoteFile, const std::string& localFile);

    bool downloadFile(const std::string& remoteFile, const std::string& localFile);

    void releaseChannel(bool isUpload);

private:
    LIBSSH2_CHANNEL* uploadChannel, *downloadChannel;
    std::string rFile, lFile;
};