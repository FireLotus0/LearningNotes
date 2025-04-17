#include "channelsession.h"
#include "utils/task/taskexecutor.h"
#include "logger/logger.h"
#include <cassert>

ChannelSession::ChannelSession(const std::string &user, const std::string &passwd, const std::string &ip, const std::string& sessionName, unsigned int id,  unsigned short sshPort)
    : Session(SessionType::SHELL, user, passwd, ip, sessionName, id, sshPort)
{
    channel = nullptr;
    channelValid = false;
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
            LOG_ERROR("Create Channel Failed!");
            return false;
        }
    } else {
        LOG_ERROR("Init Channel Not Allowed: Session Is Invalid!");
        return false;
    }
    channelValid = true;
    return channelValid;
}

bool ChannelSession::runCommand(const std::string &command) {
    isTaskSucceed = false;
    if(channelValid) {
        if (libssh2_channel_exec(channel, command.c_str()) != 0) {
            LOG_ERROR("Command Failed: command=", command, "ERROR:", libssh2_session_last_error(session, NULL, NULL, 0));
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
                    LOG_INFO("Command Finish:", resBuf.str());
                    isTaskSucceed = true;
                    break;
                } else {
                    LOG_ERROR("Command Failed: command=", command, "ERROR:", libssh2_session_last_error(session, NULL, NULL, 0));
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

SessionType ChannelSession::sessionType() const {
    return SessionType::SHELL;
}

void ChannelSession::addCreateTask() {
    addTask(TaskType::CREATE_CHANNEL, this,  &ChannelSession::initChannel);
}
