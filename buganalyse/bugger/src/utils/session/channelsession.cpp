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
    releaseChannel();
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
    bool succeed = false;
    if(channelValid) {
        if (libssh2_channel_exec(channel, command.c_str()) < 0) {
            LOG_ERROR(std::string("Command Failed: command=") + command, "ERROR:", getLastError());
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
                    succeed = true;
                    break;
                } else {
                    LOG_ERROR(std::string("Command Failed: command=") + command, "ERROR:", getLastError());
                    break;
                }
            }
        }
    }
    releaseChannel();
    return succeed;
}

void ChannelSession::addCmdTask(const std::string &cmd) {
    if(!channelValid) {
        addTask(TaskType::CREATE_CHANNEL, this, &ChannelSession::initChannel);
    }
    addTask(TaskType::RUN_CMD, this, &ChannelSession::runCommand, cmd);
}

void ChannelSession::executeCallback(TaskType taskType, unsigned long taskId, bool succeed) {
    if(isTaskTypeRemove(taskType)) {
        return;
    }
    auto iter = callbacks.find(taskType);
    if(iter != callbacks.end()) {
        auto obj = iter->second.first;
        if(taskType == CREATE_CHANNEL) {
            iter->second.second.invoke(callbacks[taskType].first, Qt::QueuedConnection, Q_ARG(int, type),Q_ARG(bool, (sessionState != SessionState::INVALID)),
                                       Q_ARG(unsigned int, id));
        } else {
            iter->second.second.invoke(obj, Qt::QueuedConnection,Q_ARG(bool , succeed), Q_ARG(QString, QString::fromStdString(resBuf.str())));
        }
    }
}

SessionType ChannelSession::sessionType() const {
    return SessionType::SHELL;
}

void ChannelSession::addCreateTask() {
    Session::addCreateTask();
    TASK_EXECUTOR.addTask(id, TaskType::CREATE_CHANNEL, &ChannelSession::initChannel, this);
}

void ChannelSession::releaseChannel() {
    if(channelValid) {
        libssh2_channel_send_eof(channel);
        libssh2_channel_wait_eof(channel);
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        channelValid = false;
    }
}
