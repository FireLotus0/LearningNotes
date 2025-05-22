
#include "sftpsession.h"
#include "utils/utils.h"
#include <fstream>

SftpSession::FileInfo::FileInfo(const std::string &name, const std::string &entry,  const LIBSSH2_SFTP_ATTRIBUTES& attr)
    : fileName(name)
    , longEntry(entry)
    , attributes(attr)
{
}

SftpSession::SftpSession(const std::string &user, const std::string &passwd, const std::string &ip, const std::string &sessionName, unsigned int id, unsigned short sshPort)
    : Session(SessionType::SFTP, user, passwd, ip, sessionName, id, sshPort)
{
}

SftpSession::~SftpSession() {
    if(sftp) {
        libssh2_sftp_shutdown(sftp);
    }
}

SessionType SftpSession::sessionType() const {
    return SessionType::SFTP;
}

bool SftpSession::initSftp() {
    sftp = libssh2_sftp_init(session);
    if (sftp == nullptr) {
        sftpValid = false;
        return false;
    }
    sftpValid = true;
    return true;
}

void SftpSession::addSftpTask(const std::string &localFile, const std::string &remoteFile, bool isUpload) {
    auto taskId = addTask(isUpload ? TaskType::SFTP_UPLOAD : TaskType::SFTP_DOWNLOAD, this, isUpload ? &SftpSession::sftpUploadFile :
    &SftpSession::sftpDownloadFile, localFile, remoteFile);
    taskInfo.insert({taskId, std::make_pair(localFile, remoteFile)});
}

bool SftpSession::sftpUploadFile(const std::string &localFile, const std::string &remoteFile) {
    if(!sftpValid) {
        return false;
    }
    auto fileData = Utils::readFile(localFile);
    if(fileData.empty()) {
        return false;
    }
    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(sftp, remoteFile.c_str(), LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC, 0644);
    if (!sftp_handle) {
        return false;
    }
    auto res = libssh2_sftp_write(sftp_handle, fileData.data(), fileData.size());
    libssh2_sftp_close(sftp_handle);
    return (res > 0 && res == fileData.size()) || res == 0;
}

bool SftpSession::sftpDownloadFile(const std::string &localFile, const std::string &remoteFile) {
    if(!sftpValid) {
        return false;
    }
    std::fstream outFile(localFile, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
    if(!outFile.is_open()) {
        return false;
    }

    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(sftp, remoteFile.c_str(), LIBSSH2_FXF_READ, 0);
    if (!sftp_handle) {
        LOG_INFO("Open SFTP Handle FAiled:", getLastError(), "Remote File:", remoteFile.c_str());
        return false;
    }

    // 读取数据并写入本地文件
    char buffer[1024];
    ssize_t nread;
    while ((nread = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer))) > 0) {
        outFile.write(buffer, nread);
    }
    outFile.close();
    libssh2_sftp_close(sftp_handle);
    return nread >= 0;
}

bool SftpSession::removeFile(const std::string &remoteFile) {
    return libssh2_sftp_unlink(sftp, remoteFile.c_str()) != 0;
}

bool SftpSession::readDir(const std::string &remoteDir) {
    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_opendir(sftp, remoteDir.c_str());
    if (!sftp_handle) {
        LOG_ERROR(std::string("Open Remote File Failed: remote dir:") + remoteDir, "ERROR:", getLastError());
        return false;
    }
    dirInfo.clear();
    char filename[512];  // 用于存储文件名
    char longentry[512]; // 用于存储长条目
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    int res;
    while ((res = libssh2_sftp_readdir_ex(sftp_handle, filename, sizeof(filename), longentry, sizeof(longentry), &attrs)) > 0) {
        FileInfo tmp(filename , longentry, attrs);
        if(tmp.fileName == "." || tmp.fileName == "..") {
            continue;
        }
//        LOG_INFO("FileName:", tmp.fileName, "Entry:", tmp.longEntry);
        dirInfo.emplace_back(std::move(tmp));
    }
    // 关闭句柄
    libssh2_sftp_closedir(sftp_handle);
    return res >= 0;
}

void SftpSession::addSftpTask(TaskType taskType, const std::string &remotePath) {
    unsigned long taskId{};
    switch(taskType) {
        case TaskType::SFTP_REMOVE_FILE:
            taskId = addTask(taskType, this, &SftpSession::removeFile, remotePath);
            break;
        case TaskType::SFTP_READ_DIR:
            taskId = addTask(taskType, this, &SftpSession::readDir, remotePath);
            break;
        default:
            assert(false);
    }
    taskInfo.insert({taskId, std::make_pair("", remotePath)});
}

void SftpSession::executeCallback(TaskType taskType, unsigned long taskId, bool succeed) {
    if(isTaskTypeRemove(taskType)) {
        return;
    }
    auto iter = callbacks.find(taskType);
    auto infoIter = taskInfo.find(taskId);
    std::pair<std::string, std::string> info;
    if(infoIter != taskInfo.end()) {
        info = infoIter->second;
        taskInfo.erase(taskId);
    }
    if(iter != callbacks.end()) {
        auto obj = iter->second.first;
        auto method = iter->second.second;
        switch(taskType) {
            case TaskType::SFTP_CREATE: {
                method.invoke(obj, Qt::QueuedConnection, Q_ARG(int, type),
                              Q_ARG(bool, sftpValid), Q_ARG(unsigned int, id));
                break;
            }
            case TaskType::SFTP_UPLOAD: {
                method.invoke(obj, Qt::QueuedConnection, Q_ARG(bool, succeed),
                              Q_ARG(QString, QString::fromStdString(info.second)), Q_ARG(QString, QString::fromStdString(info.first)), Q_ARG(bool, true));
                break;
            }
            case TaskType::SFTP_DOWNLOAD: {
                method.invoke(obj, Qt::QueuedConnection, Q_ARG(bool, succeed),
                              Q_ARG(QString, QString::fromStdString(info.second)), Q_ARG(QString, QString::fromStdString(info.first)), Q_ARG(bool, false));
                break;
            }
            case TaskType::SFTP_READ_DIR: {
                method.invoke(obj, Qt::QueuedConnection, Q_ARG(bool, succeed), Q_ARG(QString, QString::fromStdString(info.second)),
                              Q_ARG(QVector<SftpSession::FileInfo>, QVector<SftpSession::FileInfo>::fromStdVector(dirInfo)));
                break;
            }
            case TaskType::SFTP_REMOVE_FILE: {
                method.invoke(obj, Qt::QueuedConnection, Q_ARG(bool, succeed), Q_ARG(QString, QString::fromStdString(info.second)));
                break;
            }
        }
        dirInfo.clear();
    }
}

void SftpSession::addCreateTask() {
    Session::addCreateTask();
    TASK_EXECUTOR.addTask(id, TaskType::SFTP_CREATE, &SftpSession::initSftp, this);
}
