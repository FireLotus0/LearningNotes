#pragma once

#include "utils/session/scpsession.h"
#include "utils/session/channelsession.h"

#include <qobject.h>
#include <qmap.h>

class SessionManager : public QObject {
Q_OBJECT
public:
    explicit SessionManager(QObject *parent = nullptr);

    QPair<QString, unsigned> createSession(SessionType sessionType, const QString &user, const QString &passwd, const QString &ip, const QString &sessionName = "", unsigned short sshPort = 22);

    void closeSession(unsigned int sessionId);

    void scpTransfer(unsigned int sessionId, const QString& localFile, const QString& remoteFile, bool isUpload);

    void executeShellCmd(unsigned int sessionId, const QString& cmd);

public slots:
    void onScpUploadFinished(bool success, const QString &remoteFile, const QString &localFile);

    void onScpDownloadFinished(bool success, const QString &remoteFile, const QString &localFile);

    void onCommandFinished(bool success, const QString &output);

    void onRemoveSessionTask(unsigned int sessionId);

private:
    QMap<unsigned, Session *> sessions;
};
