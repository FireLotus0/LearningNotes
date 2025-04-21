#pragma once

#include "utils/session/scpsession.h"
#include "utils/session/channelsession.h"
#include "utils/session/sftpsession.h"

#include <qobject.h>
#include <qmap.h>

Q_DECLARE_METATYPE(SftpSession::FileInfo)

class SessionManager : public QObject {
Q_OBJECT
public:
    static SessionManager* instance();

    QPair<QString, unsigned> createSession(SessionType sessionType, const QString &user, const QString &passwd, const QString &ip, const QString &sessionName = "", unsigned short sshPort = 22);

    void closeSession(unsigned int sessionId);

    void scpTransfer(unsigned int sessionId, const QString& localFile, const QString& remoteFile, bool isUpload);

    void sftpTransfer(unsigned int sessionId, const QString& localFile, const QString& remoteFile, bool isUpload);

    void sftpReadDirInfo(unsigned int sessionId, const QString& dirName);

    void sftpRemoveFile(unsigned int sessionId, const QString& remoteFile);

    void executeShellCmd(unsigned int sessionId, const QString& cmd);

    template<typename T>
    T* getSession(unsigned int sessionId) {
        if(sessions.contains(sessionId)) {
            return dynamic_cast<T*>(sessions[sessionId]);
        }
        return nullptr;
    }

signals:
    void sigScpUploadFinished(bool success, const QString &remoteFile, const QString &localFile);
    void sigScpDownloadFinished(bool success, const QString &remoteFile, const QString &localFile);
    void sigCommandFinished(bool success, const QString &output);
    void sigRemoveSessionTask(unsigned int sessionId);
    void sigSftpTransferFinished(bool success, const QString& remoteFile, const QString& localFile, bool isUpload);
    void sigSftpReadDirFinished(bool success, const QString& remoteDir, const QVector<SftpSession::FileInfo>& data);
    void sigSftpRemoveFileFinished(bool success, const QString& remoteFile);
    void sigSessionConnectFinished(int type, bool success, unsigned int sessionId);

public slots:
    void onScpUploadFinished(bool success, const QString &remoteFile, const QString &localFile);

    void onScpDownloadFinished(bool success, const QString &remoteFile, const QString &localFile);

    void onCommandFinished(bool success, const QString &output);

    void onRemoveSessionTask(unsigned int sessionId);

    void onSftpTransferFinished(bool success, const QString& remoteFile, const QString& localFile, bool isUpload);

    void onSftpReadDirFinished(bool success, const QString& remoteDir, const QVector<SftpSession::FileInfo>& data);

    void onSftpRemoveFileFinished(bool success, const QString& remoteFile);

    void onSessionConnectFinished(int type, bool success, unsigned int sessionId);
private:
    explicit SessionManager(QObject *parent = nullptr);
private:
    QMap<unsigned, Session *> sessions;
};

#define SMPTR (SessionManager::instance())