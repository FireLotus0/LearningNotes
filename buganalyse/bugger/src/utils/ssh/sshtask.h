#pragma once
#include <qobject.h>
#include <libssh2.h>
#include <QFutureWatcher>
#include <libssh2_sftp.h>
#include <QtConcurrent>

class Session;
class SshTask : public QObject {
Q_OBJECT
using CPTR = const char*;
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
    explicit SshTask(Session* session, TaskType type,  QObject *parent = nullptr);

    ~SshTask();

    bool isTaskBlocked(TaskType type);

    void doTask();
signals:
    void taskFinished(TaskType type);

    void errorOccurred(const QString &error);

protected slots:
//    virtual void onTaskFinished();

private:
    template<typename T, typename Func, typename...Args>
    std::enable_if_t<std::is_member_function_pointer_v<Func>, void> executeAsync(Func func, Args&&...args) {
        auto watcher = new QFutureWatcher<T>;
        connect(watcher, &QFutureWatcher<int>::finished, this, [&, watcher]{
//            onTaskFinished();
            printResult(taskType, isTaskSucceed(watcher->result()));
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run(this, func, std::forward<Args>(args)...));
    }
    //&& std::is_invocable_v<Func, Args...>
    template<typename T, typename Func, typename...Args>
    std::enable_if_t<!std::is_member_function_pointer_v<Func>, void> executeAsync(Func func, Args&&...args) {
        auto watcher = new QFutureWatcher<T>;
        connect(watcher, &QFutureWatcher<int>::finished, this, [&, watcher]{
//            onTaskFinished();
            printResult(taskType, isTaskSucceed(watcher->result()));
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run(func, std::forward<Args>(args)...));
    }

    template<typename T>
    bool isTaskSucceed(T val) {
        if(taskType == SERVER_CONNECT) {
            return val > 0;
        } else if(std::is_same_v<T, int>) {
            return val == 0;
        }
    }

private:
    void printResult(TaskType type, bool res);

    int connectToServer();

    int authUserPasswd();
private:
    TaskType taskType;
    size_t bufferSz;
    int errBufSz = 512;
    char *buf, *errBuf;
    Session* session;
};
