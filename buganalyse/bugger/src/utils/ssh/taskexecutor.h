#pragma once

#include <qobject.h>
#include <libssh2.h>
#include <QFutureWatcher>
#include <libssh2_sftp.h>
#include <QtConcurrent>

//        libssh2_session_disconnect(session, "Normal Shutdown");
//        libssh2_session_free(session);
//        closesocket(sock);
//        libssh2_exit();
//        WSACleanup();
class Session;

class TaskExecutor : public QObject
{
    Q_OBJECT
public:
    enum TaskType {
        // 连接操作
        SERVER_CONNECT,         // 连接服务器
        SSH_CONNECT,            // 创建SSH连接
        SSH_HAND_SHAKE,         // SSH握手
        PASSWD_VERIFY,          // 密码验证 LIBSSH2_SESSION *session, const char *username, const char *password
        PUB_KEY_VERIFY,         // 公钥验证 LIBSSH2_SESSION *session, const char *username, const char *publickey, const char *privatekey, const char *passphrase
        // 通道操作
        OPEN_CHANNEL,           // 打开通道 LIBSSH2_CHANNEL *(LIBSSH2_SESSION *session)
        EXECUTE_CMD,            // 执行远程命令   LIBSSH2_CHANNEL *channel, const char *command
        READ_DATA,              // 读取数据 LIBSSH2_CHANNEL *channel, void *buf, size_t buflen
        WRITE_DATA,             // 写入数据 LIBSSH2_CHANNEL *channel, const void *buf, size_t buflen
        CHANNEL_FORWARD,        // TCP隧道转发 LIBSSH2_CHANNEL *channel, const char *host, int port
        // SFTP操作
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
public:
    explicit TaskExecutor(QObject* parent = nullptr);

    ~TaskExecutor() override;

    template<typename...Args>
    void addTask(TaskType taskType, Args&&...args) {
        auto func = [this, taskType, &args...] {
            executeTask(taskType, std::forward<Args>(args)...);
        };
        taskCache.push_back({taskType, func});
        if(isFirst) {
//            std::invoke(taskCache.head().second);
            taskCache.head().second();
            isFirst = false;
        }
    }

private:
    template<typename...Args>
    void executeTask(TaskType taskType, Args&&...args);

    template<typename Func, typename...Args>
    std::enable_if_t<std::is_member_function_pointer_v<Func>, void> executeAsync(TaskType taskType, Func func, Args &&...args) {
        using RT = std::invoke_result_t<Func, decltype(this), Args...>;
        getWatcher<RT>(taskType)->setFuture(QtConcurrent::run(this, func, std::forward<Args>(args)...));
    }

    template<typename Func, typename...Args>
    std::enable_if_t<!std::is_member_function_pointer_v<Func>, void> executeAsync(TaskType taskType, Func func, Args &&...args) {
        using RT = std::invoke_result_t<Func, Args...>;
        getWatcher<RT>(taskType)->setFuture(QtConcurrent::run(func, std::forward<Args>(args)...));
    }

    template<typename T>
    bool isTaskSucceed(T val, TaskType taskType) {
        if constexpr (std::is_pointer_v<T>) {
            return val != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            switch (taskType) {
                case SERVER_CONNECT:
                    return val > 0;
                default:
                    return val == 0;
            }
        }
    }

    template<typename T>
    QFutureWatcher<T>* getWatcher(TaskType taskType) {
        QString typeName = typeid(T).name();
        if(!watchers.contains(typeName)) {
            auto watcher = new QFutureWatcher<T>;
            watchers[typeName] = (QFutureInterfaceBase*)(watcher);
            connect(watcher, &QFutureWatcher<T>::finished, this, [&, taskType, watcher] {
                printResult(taskType, isTaskSucceed(watcher->result(), taskType), WSAGetLastError());
            });
        }
        return (QFutureWatcher<T>*)(watchers[typeName]);
    }

signals:
    void taskFinished(TaskType type);

    void errorOccurred(const QString &error);

private:
    void printResult(TaskType type, bool res, int errCode = 0);

    int connectToServer(SOCKET* socketPtr, const QString& ip);

    int authUserPasswd(LIBSSH2_SESSION* session, const QString& user, const QString& passwd);

    void onTaskFinished(TaskType type, bool res);

    LIBSSH2_CHANNEL* openChannel(LIBSSH2_SESSION* session, LIBSSH2_CHANNEL *&channel);

private:
    size_t bufferSz = 1024;
    int errBufSz = 512;
    char *buf, *errBuf;
//    Session *session;
    QMap<QString, QFutureInterfaceBase*> watchers;
    QQueue<QPair<TaskType, std::function<void()>>> taskCache;

    bool isFirst = true;
};

template<typename...Args>
void TaskExecutor::executeTask(TaskType taskType, Args&&...args) {
    switch (taskType) {
        case SERVER_CONNECT:
            executeAsync(taskType, &TaskExecutor::connectToServer, std::forward<Args>(args)...);
            break;
        case SSH_HAND_SHAKE:
//            executeAsync(taskType, &libssh2_session_handshake,  std::forward<Args>(args)...);
            break;
        case PASSWD_VERIFY:
//            executeAsync(taskType, &TaskExecutor::authUserPasswd,  std::forward<Args>(args)...);
        default:
            break;
    }
}

