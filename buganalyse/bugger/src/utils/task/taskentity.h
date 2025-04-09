#pragma once

#include <type_traits>
#include <tuple>
#include <string>

#define MAKE_ITEM(GEN) \
    GEN(INIT_CONNECTION), \
    GEN(SERVER_CONNECT), \
    GEN(SSH_HAND_SHAKE), \
    GEN(PASSWD_VERIFY), \
    GEN(PUB_KEY_VERIFY), \
    GEN(OPEN_CHANNEL), \
    GEN(EXECUTE_CMD), \
    GEN(READ_DATA), \
    GEN(WRITE_DATA), \
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
    MAKE_ITEM(TASK_ENUM)
};

#define TASK_NAME(x) #x
const char* TASK_NAME[] = {
    MAKE_ITEM(TASK_NAME)
};

struct TaskEntityBase {
    explicit TaskEntityBase(TaskType taskType);

    template<typename T>
    bool isSucceed(T v) {
        if constexpr (std::is_pointer_v<T>) {
            return v != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        }
    }

    virtual bool operator()() {
        return true;
    }

    std::string taskResultStr(bool result) const;

    TaskType taskType;
};

template<typename Func, typename...Args>
struct TaskEntity : public TaskEntityBase {
    explicit TaskEntity(TaskType taskType, Func f, Args&&...args) : TaskEntityBase(taskType){
        func = f;
        params = std::forward_as_tuple(std::forward<Args>(args)...);
    }

    auto operator()() override{
        return this->isSucceed(std::apply(func, params));
    }

private:
    Func func;
    std::tuple<Args...> params;
};






