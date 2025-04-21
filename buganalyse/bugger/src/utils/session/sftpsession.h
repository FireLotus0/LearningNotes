#pragma once

#pragma once

#include "session.h"

class SftpSession : public Session
{
public:
    struct FileInfo{
        FileInfo() = default;
        FileInfo(const std::string& name, const std::string& entry, const LIBSSH2_SFTP_ATTRIBUTES& attr);
        std::string fileName, longEntry;
        LIBSSH2_SFTP_ATTRIBUTES attributes;
    };
public:
    SftpSession(const std::string& user, const std::string& passwd, const std::string& ip, const std::string& sessionName, unsigned int id, unsigned short sshPort = 22);

    ~SftpSession() override;

    void addSftpTask(const std::string& localFile, const std::string& remoteFile, bool isUpload);

    void addSftpTask(TaskType taskType, const std::string& arg);

    void executeCallback(TaskType taskType, unsigned long taskId, bool succeed) override;

    SessionType sessionType() const override;

    void addCreateTask() override;

private:
    bool initSftp();

    bool sftpUploadFile(const std::string& localFile, const std::string& remoteFile);

    bool sftpDownloadFile(const std::string& localFile, const std::string& remoteFile);

    bool removeFile(const std::string& remoteFile);

    bool readDir(const std::string& remoteDir);
private:
    LIBSSH2_SFTP *sftp{nullptr};
    bool sftpValid = false;
    std::vector<FileInfo> dirInfo;
    std::unordered_map<unsigned long, std::pair<std::string, std::string>> taskInfo;
};
