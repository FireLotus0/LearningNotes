#include "scpsession.h"
#include "utils/utils.h"

#include <libssh2.h>
#include <cassert>
#include <fstream>

ScpSession::ScpSession(const std::string &user, const std::string &passwd, const std::string &ip, const std::string& sessionName, unsigned int id, unsigned short sshPort)
    : Session(SessionType::SCP, user, passwd, ip, sessionName, id, sshPort)
{
    uploadChannel = nullptr;
    downloadChannel = nullptr;
}

ScpSession::~ScpSession() {
    releaseChannel(true);
    releaseChannel(false);
}

void ScpSession::addScpTask(const std::string &remoteFile, const std::string &localFile, bool isUpload) {
        addTask(isUpload ? TaskType::SCP_UPLOAD : TaskType::SCP_DOWNLOAD, this, (isUpload ? &ScpSession::uploadFile : &ScpSession::downloadFile),
                remoteFile, localFile);
        rFile = remoteFile;
        lFile = localFile;
}

bool ScpSession::uploadFile(const std::string &remoteFile, const std::string &localFile) {
    isTaskSucceed = false;
    auto fileData = Utils::readFile(localFile);
    if (fileData.empty()) {
        std::cerr << __FILE__ << __LINE__ << " Scp Upload File Failed: Local File Is Empty Or Not Exist! LocalFile=" << localFile << std::endl;
        return isTaskSucceed;
    }
    assert(uploadChannel == nullptr);
    auto tm_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    uploadChannel = libssh2_scp_send64(session, remoteFile.c_str(), 0644, fileData.size(), tm_t, tm_t);
    if (!uploadChannel) {
        std::cerr << __FILE__ << __LINE__ << " Scp Upload File Failed: Create Channel Failed: " << libssh2_session_last_error(session, NULL, NULL, 0) << std::endl;
    } else {
        auto rt = libssh2_channel_write(uploadChannel, fileData.data(), fileData.size() < 0);
        if (rt < 0) {
            std::cerr << __FILE__ << __LINE__ << " Scp Upload File Failed: Write Data Failed: " << libssh2_session_last_error(session, NULL, NULL, 0) << std::endl;
        }
        releaseChannel(true);
        isTaskSucceed = rt > 0;
    }
    return isTaskSucceed;
}

bool ScpSession::downloadFile(const std::string &remoteFile, const std::string &localFile) {
    isTaskSucceed = false;
    std::fstream outFile(localFile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    if(!outFile.is_open()) {
        std::cerr <<__FILE__ << __LINE__ << " Write File Failed：Open " << localFile << std::endl;
        return isTaskSucceed;
    }
    downloadChannel = libssh2_scp_recv2(session, remoteFile.c_str(), NULL);
    if(!downloadChannel) {
        std::cerr <<__FILE__ << __LINE__ << " Create Scp Channel Failed!" << std::endl;
        outFile.close();
        return isTaskSucceed;
    }

    char buffer[4096];
    std::size_t readCounts;
    while((readCounts = libssh2_channel_read(downloadChannel, buffer, 4096)) > 0) {
        outFile.write(buffer, readCounts);
    }
    outFile.close();
    releaseChannel(false);
    isTaskSucceed = true;
    return isTaskSucceed;
}

void ScpSession::releaseChannel(bool isUpload) {
    if (isUpload && uploadChannel) {
        // 释放上传通道
        libssh2_channel_send_eof(uploadChannel);
        libssh2_channel_wait_eof(uploadChannel);
        libssh2_channel_close(uploadChannel);
        libssh2_channel_free(uploadChannel);
        uploadChannel = nullptr;
    } else if (!isUpload && downloadChannel) {
        // 释放下载通道
        libssh2_channel_send_eof(downloadChannel);
        libssh2_channel_wait_eof(downloadChannel);
        libssh2_channel_close(downloadChannel);
        libssh2_channel_free(downloadChannel);
        downloadChannel = nullptr;
    }
}

void ScpSession::executeCallback(TaskType taskType) {
    if(isTaskTypeRemove(taskType)) {
        return;
    }
    auto iter = callbacks.find(taskType);
    if(iter != callbacks.end()) {
        auto obj = iter->second.first;
        iter->second.second.invoke(obj, Qt::QueuedConnection, Q_ARG(bool, isTaskSucceed), Q_ARG(QString, QString::fromStdString(rFile)),
                                   Q_ARG(QString, QString::fromStdString(lFile)));
    }
}
