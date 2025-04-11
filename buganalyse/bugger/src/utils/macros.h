#pragma once
#include <string>

/*------ Session Type Define ------*/
#define MAKE_SESSION_TYPE(GEN) \
    GEN(SFTP),            \
    GEN(SCP),             \
    GEN(SHELL)

#define SESSION_ENUM(x) x
enum SessionType {
    MAKE_SESSION_TYPE(SESSION_ENUM)
};
/*-------------------------------*/

#define SESSION_NAME(x) #x
static std::string SessionName[] = {
        MAKE_SESSION_TYPE(SESSION_NAME)
};

/*------ Task Type Define ------*/
#define MAKE_TASK_TYPE(GEN) \
    GEN(REMOVE_ALL_TASK), \
    GEN(INIT_CONNECTION), \
    GEN(CREATE_CHANNEL), \
    GEN(RUN_CMD), \
    GEN(SCP_UPLOAD), \
    GEN(SCP_DOWNLOAD), \
    GEN(CHANNEL_FORWARD), \
    GEN(SFTP_CREATE), \
    GEN(SFTP_OPEN_FILE), \
    GEN(SFTP_READ_FILE), \
    GEN(SFTP_WRITE_FILE), \
    GEN(SFTP_CLOSE_FILE), \
    GEN(SFTP_REMOVE_FILE), \
    GEN(SFTP_MK_DIR), \
    GEN(SFTP_OPEN_DIR), \
    GEN(SFTP_READ_DIR)

#define TASK_ENUM(x) x
enum TaskType {
    MAKE_TASK_TYPE(TASK_ENUM)
};

#define TASK_NAME(x) #x
static std::string TASK_NAME[] = {
        MAKE_TASK_TYPE(TASK_NAME)
};
/*-------------------------------*/

#define MAKE_SESSION_STATE(GEN) \
    GEN(IDLE), \
    GEN(RUNNING), \
    GEN(INVALID)

#define SESSION_STATE_ENUM(x) x
enum SessionState {
    MAKE_SESSION_STATE(SESSION_STATE_ENUM)
};

#define SESSION_STATE_NAME(x) #x
static std::string SessionStateName[] = {
        MAKE_SESSION_STATE(SESSION_STATE_NAME)
};