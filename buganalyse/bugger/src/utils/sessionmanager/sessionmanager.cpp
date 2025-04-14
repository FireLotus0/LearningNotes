#include "sessionmanager.h"
#include "dialog/info/infodlg.h"

#include <quuid.h>
#include <qdebug.h>
#include <qdatetime.h>
#include <qcryptographichash.h>

SessionManager::SessionManager(QObject *parent) : QObject(parent) {

}

QPair<QString, unsigned>
SessionManager::createSession(SessionType sessionType, const QString &user, const QString &passwd, const QString &ip,
                              const QString &sessionName, unsigned short sshPort) {
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    auto str = QString::number(timestamp);
    auto hashRes = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256);
    auto uuidBytes = hashRes.left(16);
    auto uuidStr = QUuid::fromRfc4122(uuidBytes).toString();
    QString name = (sessionName.isEmpty() ? user + "@" + "ip" : sessionName) + uuidStr.mid(1, uuidStr.size() - 2);
    qDebug() << "name is " << name;
    auto sessionId = qHash(name);
    Q_ASSERT(!sessions.contains(sessionId));
    Session *session{};
    switch (sessionType) {
        case SessionType::SHELL: {
            session = new ChannelSession(user.toStdString(), passwd.toStdString(), ip.toStdString(), name.toStdString(),
                                         sessionId, sshPort);
            session->registerCallback(TaskType::RUN_CMD, this, "onCommandFinished(bool,QString)");
            break;
        }
        case SessionType::SCP: {
            session = new ScpSession(user.toStdString(), passwd.toStdString(), ip.toStdString(), name.toStdString(),
                                     sessionId, sshPort);
            session->registerCallback(TaskType::SCP_UPLOAD, this, "onScpUploadFinished(bool,QString,QString)");
            session->registerCallback(TaskType::SCP_DOWNLOAD, this, "onScpDownloadFinished(bool,QString,QString)");
            break;
        }
        case SessionType::SFTP: {
            session = new SftpSession(user.toStdString(), passwd.toStdString(), ip.toStdString(), name.toStdString(),
                                     sessionId, sshPort);
            session->registerCallback(TaskType::SFTP_UPLOAD, this, "onSftpTransferFinished(bool,QString,QString)");
            session->registerCallback(TaskType::SFTP_DOWNLOAD, this, "onSftpTransferFinished(bool,QString,QString)");
            session->registerCallback(TaskType::SFTP_READ_DIR, this, "onSftpReadDirFinished(bool,QString,QVector<SftpSession::FileInfo>)");
            session->registerCallback(TaskType::SFTP_REMOVE_FILE, this, "onSftpRemoveFileFinished(bool,QString)");
            break;
        };
        default:
            Q_ASSERT(false);
    }
    session->registerCallback(TaskType::REMOVE_ALL_TASK, this, "onRemoveSessionTask(unsigned int)");
    sessions[sessionId] = session;
    return qMakePair(name, sessionId);
}

void SessionManager::closeSession(unsigned int sessionId) {
    Q_ASSERT(sessions.contains(sessionId));
    auto session = sessions[sessionId];
    session->addTask(TaskType::REMOVE_ALL_TASK, session, nullptr);
}

void SessionManager::onRemoveSessionTask(unsigned int sessionId) {
    Q_ASSERT(sessions.contains(sessionId));
    delete sessions[sessionId];
    sessions.remove(sessionId);
}

void SessionManager::onScpUploadFinished(bool success, const QString &remoteFile, const QString &localFile) {
    InfoDlg infoDlg;
    QString info;
    if (success) {
        info = localFile + " 已上传至 " + remoteFile;
    } else {
        info = "SCP文件上传失败:本地路径: " + localFile + " 远程路径:" + remoteFile;
    }
    infoDlg.setStateStr(!success, info);
    infoDlg.exec();
}

void SessionManager::onScpDownloadFinished(bool success, const QString &remoteFile, const QString &localFile) {
    InfoDlg infoDlg;
    QString info;
    if(success) {
        info = remoteFile + " 已下载至 " + localFile;
    } else {
        info = "SCP文件下载失败: 远程路径: " + remoteFile + " 下载路径: " + localFile;
    }
    infoDlg.setStateStr(!success, info);
    infoDlg.exec();
}

void SessionManager::onCommandFinished(bool success, const QString &output) {
    InfoDlg infoDlg;
    QString info;
    infoDlg.setStateStr(!success, "命令执行结果:" + output);
    infoDlg.exec();
}

void SessionManager::scpTransfer(unsigned int sessionId, const QString &localFile, const QString &remoteFile,
                                 bool isUpload) {
    Q_ASSERT(sessions[sessionId]);
    qDebug() << QString::fromStdString(SessionName[sessions[sessionId]->sessionType()]);
    auto session = dynamic_cast<ScpSession *>(sessions[sessionId]);
    session->addScpTask(remoteFile.toStdString(), localFile.toStdString(), isUpload);
}

void SessionManager::executeShellCmd(unsigned int sessionId, const QString &cmd) {
    Q_ASSERT(sessions[sessionId]);
    qDebug() << QString::fromStdString(SessionName[sessions[sessionId]->sessionType()]);
    auto session = dynamic_cast<ChannelSession *>(sessions[sessionId]);
    session->addCmdTask(cmd.toStdString());
}

void SessionManager::onSftpTransferFinished(bool success, const QString &remoteFile, const QString &localFile) {

}

void SessionManager::onSftpReadDirFinished(bool success, const QString &remoteDir,
                                           const QVector<SftpSession::FileInfo> &data) {

}

void SessionManager::onSftpRemoveFileFinished(bool success, const QString &remoteFile) {

}
