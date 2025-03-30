#pragma once

#include <qobject.h>
#include <libssh2.h>
#include <QFutureWatcher>
#include <libssh2_sftp.h>
#include <QtConcurrent>

struct TaskEntityBase {
    template<typename T>
    bool isSucceed(T v) {
        if constexpr (std::is_pointer_v<T>) {
            return v != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        }
    }
};

template<typename Func, typename...Args>
struct TaskEntity : public TaskEntityBase {
    explicit TaskEntity(Func f, Args...args) {
        func = [f, args...]()mutable {
            return f(std::forward<Args>(args)...);
        };
    }

    std::decay_t<std::invoke_result_t<Func, Args...>> operator()() {
        return std::invoke(func);
    }

private:
    Func func;
};

template<typename...Bases>
struct ComposedTask : public Bases ... {
    using Bases::operator()...;

    int operator()() {
        return (... && isSucceed(Bases::operator()()));
    }
};


template<typename...Args>
void printVarArgs(Args...args) {
    auto tmp = {(qDebug() << "Args: " << &args << " ", 0)...};
}

class TaskExecutor : public QObject {
Q_OBJECT
public:
    enum TaskType {
        NONE,
        INIT_CONNECTION,
        /// 连接操作
        SERVER_CONNECT,         // 连接服务器
        SSH_HAND_SHAKE,         // SSH握手
        PASSWD_VERIFY,          // 密码验证 LIBSSH2_SESSION *session, const char *username, const char *password
        PUB_KEY_VERIFY,         // 公钥验证 LIBSSH2_SESSION *session, const char *username, const char *publickey, const char *privatekey, const char *passphrase
        /// 通道操作
        OPEN_CHANNEL,           // 打开通道 LIBSSH2_CHANNEL *(LIBSSH2_SESSION *session)
        EXECUTE_CMD,            // 执行远程命令   LIBSSH2_CHANNEL *channel, const char *command
        READ_DATA,              // 读取数据 LIBSSH2_CHANNEL *channel, void *buf, size_t buflen
        WRITE_DATA,             // 写入数据 LIBSSH2_CHANNEL *channel, const void *buf, size_t buflen
        CHANNEL_FORWARD,        // TCP隧道转发 LIBSSH2_CHANNEL *channel, const char *host, int port
        /// SFTP操作
        SFTP_CREATE,            // SFTP会话创建 LIBSSH2_SFTP *libssh2_sftp_init(LIBSSH2_SESSION *session);
        SFTP_OPEN_FILE,         // 打开文件 LIBSSH2_SFTP_HANDLE *libssh2_sftp_open(LIBSSH2_SFTP *sftp, const char *path, int flags, int mode);
        SFTP_READ_FILE,         // 读取文件 int libssh2_sftp_read(LIBSSH2_SFTP_HANDLE *handle, void *buf, size_t buflen);
        SFTP_WRITE_FILE,        // 写入文件 int libssh2_sftp_write(LIBSSH2_SFTP_HANDLE *handle, const void *buf, size_t buflen);
        SFTP_CLOSE_FILE,        // 关闭文件 int libssh2_sftp_close(LIBSSH2_SFTP_HANDLE *handle);
        SFTP_REMOVE_FILE,       // 移除文件 int libssh2_sftp_remove(LIBSSH2_SFTP *sftp, const char *filename);
        SFTP_MK_DIR,            // 创建目录 int libssh2_sftp_mkdir(LIBSSH2_SFTP *sftp, const char *path, int mode);
        SFTP_OPEN_DIR,          // 列出目录内容 LIBSSH2_SFTP_HANDLE *libssh2_sftp_opendir(LIBSSH2_SFTP *sftp, const char *path);
        SFTP_READ_DIR,          // 读取目录项 int libssh2_sftp_readdir(LIBSSH2_SFTP_HANDLE *handle, char *buf, size_t buflen);
    };

    Q_ENUM(TaskType)

    enum ErrorLevel {
        NORMAL,
        FATAL
    };

    Q_ENUM(ErrorLevel)

public:
    explicit TaskExecutor(QObject *parent = nullptr);

    ~TaskExecutor() override;

    /**
     * @brief 添加SSH任务
     * @param TaskType 任务类型
     * @param Args 调用参数
     */
    template<TaskType taskType, typename...Args>
    void addTask(Args &&...args);

    void cancelCurTask();

    void executeCurTask();

    ErrorLevel getErrLelFromTask(TaskType taskType);

private:
    /**
     * @brief 异步执行任务
     * @param Func 成员函数指针
     * @param Args 函数参数
     */
    template<typename Func, typename...Args>
    std::enable_if_t<std::is_member_function_pointer_v<Func>, void>
    executeAsync(TaskType taskType, Func func, Args &&...args) {
        using RT = std::invoke_result_t<Func, TaskExecutor, Args...>;
        curTask = taskType;
        watchFuture<RT>(taskType)->setFuture(QtConcurrent::run(this, func, std::forward<Args>(args)...));
    }

    /**
    * @brief 异步执行任务
    * @param Func 普通函数指针
    * @param Args 函数参数
    */
    template<typename Func, typename...Args>
    std::enable_if_t<!std::is_member_function_pointer_v<Func>, void>
    executeAsync(TaskType taskType, Func func, Args &&...args) {
        using RT = std::invoke_result_t<Func, Args...>;
        curTask = taskType;
        watchFuture<RT>(taskType)->setFuture(QtConcurrent::run(func, std::forward<Args>(args)...));
    }

    /**
    * @brief 判断任务执行结果
    * @param T 任务函数返回值
    * @param TaskType 任务类型
    */
    template<typename T>
    bool isTaskSucceed(T val, TaskType taskType) {
        bool res;
        if constexpr (std::is_pointer_v<T>) {
            res = val != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            res = val == 0;
        }
        return res;
    }

    /**
    * @brief 根据任务函数返回值获取对应Watcher
    * @param T 函数返回值类型
    * @param TaskType 任务类型
    */
    template<typename T>
    QFutureWatcher<T> *watchFuture(TaskType taskType) {
        QString typeName = typeid(T).name();
        if (!watchers.contains(typeName)) {
            auto watcher = new QFutureWatcher<T>;
            watchers[typeName] = (QFutureInterfaceBase *) (watcher);
            connect(watcher, &QFutureWatcher<T>::finished, this, [&, taskType, watcher] {
                onTaskFinished(curTask, isTaskSucceed(watcher->result(), curTask));
            });
        }
        return (QFutureWatcher<T> *) watchers[typeName];
    }

    /**
     * @brief 当前任务执行完成回调，继续执行队列任务
     */
    void onTaskFinished(TaskType type, bool res);

signals:
    void taskFinished(TaskType type, bool res);

private:
   int initConnection(SOCKET sock, const QString& ip, LIBSSH2_SESSION *session, const QString& user, const QString& passwd);

private:
    // socket网络连接
    int connectToServer(SOCKET sock, const QString &ip);

    // ssh认证用户名，密码
    int authUserPasswd(LIBSSH2_SESSION *session, const QString &user, const QString &passwd);

    // 创建通道
    LIBSSH2_CHANNEL *openChannel(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL **channel);

private:
    size_t bufferSz = 1024; // 数据缓冲区
    int errBufSz = 512;     // 错误信息缓冲区
    char *buf, *errBuf;
    QMap<QString, QFutureInterfaceBase *> watchers; // {typeid.name, QFutureWatcher<T>*}
    QQueue<QPair<TaskType, std::function<void()>>> taskCache;   // 任务队列
    TaskType curTask = NONE;
};

template<TaskExecutor::TaskType taskType, typename...Args>
void TaskExecutor::addTask(Args &&...args) {
//    if(taskType == OPEN_CHANNEL) {
//        printVarArgs(std::forward<Args>(args)...);
//    }
    auto func = [this, args...]() mutable {
        if constexpr (taskType == INIT_CONNECTION) {
            executeAsync(taskType, &TaskExecutor::initConnection, std::forward<Args>(args)...);
        } else if constexpr (taskType == OPEN_CHANNEL) {
            executeAsync(taskType, &TaskExecutor::openChannel, std::forward<Args>(args)...);
        }
    };
    bool needInvoke = taskCache.isEmpty();
    taskCache.push_back({taskType, func});
    if (needInvoke) {
        std::invoke(taskCache.head().second);
    }
}

struct TaskFactory {
    auto getTaskObject(TaskExecutor::TaskType...) {

    }
};