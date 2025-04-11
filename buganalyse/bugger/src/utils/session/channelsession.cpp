#include <cassert>
#include "channelsession.h"
#include "utils/task/taskexecutor.h"

ChannelSession::ChannelSession(const std::string &user, const std::string &passwd, const std::string &ip, const std::string& sessionName, unsigned int id,  unsigned short sshPort)
    : Session(SessionType::SHELL, user, passwd, ip, sessionName, id, sshPort)
{
    channel = nullptr;
    channelValid = false;
    addTask(TaskType::CREATE_CHANNEL, this,  &ChannelSession::initChannel);
}

ChannelSession::~ChannelSession() {
    if(channelValid) {
        libssh2_channel_send_eof(channel);
        libssh2_channel_wait_eof(channel);
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
    }
}

bool ChannelSession::initChannel() {
    if(isSessionValid()) {
        channel = libssh2_channel_open_session(session);
        if (!channel) {
            std::cerr << __FILE__ << ":" << __LINE__ << " Create Channel Failed!" << std::endl;
            return false;
        }
    } else {
        std::cerr << __FILE__ << ":" << __LINE__ << " Init Channel Not Allowed: Session Is Invalid!" << std::endl;
        return false;
    }
    channelValid = true;
    return channelValid;
}

bool ChannelSession::runCommand(const std::string &command) {
    isTaskSucceed = false;
    if(channelValid) {
        if (libssh2_channel_exec(channel, command.c_str()) != 0) {
            std::cerr << __FILE__ << ":" << __LINE__ << " Command Failed: CMD=" << command << " Err=" << libssh2_session_last_error(session, NULL, NULL, 0) << std::endl;
        } else {
            int bytes_read;
            char buffer[1024];
            resBuf.clear();
            while (true) {
                bytes_read = libssh2_channel_read(channel, buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0'; // 确保字符串结束
                    resBuf << buffer;
                } else if (bytes_read == 0) {
                    std::cout << "Command Finish: " << resBuf.str() << std::endl;
                    isTaskSucceed = true;
                    break;
                } else {
                    std::cerr << __FILE__ << ":" << __LINE__ << " Command Failed: CMD=" << command << " Err=" << libssh2_session_last_error(session, NULL, NULL, 0) << std::endl;
                    isTaskSucceed = false;
                    break;
                }
            }
        }
    }
    return isTaskSucceed;
}

void ChannelSession::addCmdTask(const std::string &cmd) {
    addTask(TaskType::RUN_CMD, this, &ChannelSession::runCommand, cmd);
}

void ChannelSession::executeCallback(TaskType taskType) {
    if(isTaskTypeRemove(taskType)) {
        return;
    }
    auto iter = callbacks.find(taskType);
    if(iter != callbacks.end()) {
        auto obj = iter->second.first;
        iter->second.second.invoke(obj, Qt::QueuedConnection, Q_ARG(bool , isTaskSucceed), Q_ARG(QString, QString::fromStdString(resBuf.str())));
    }
}
