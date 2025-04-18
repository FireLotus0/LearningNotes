#pragma once

#include "src/widgets/frame/windowframelesshelper.h"
#include "ui_login.h"
#include <qdialog.h>

class LoginDlg : public WindowFramelessWidget<QDialog> {
Q_OBJECT

    enum Error {
        NONE,
        IP_WRONG,
        AUTH_WRONG,
        IP_NONE,
        USER_NONE,
        PASSWD_NONE
    };
public:
    explicit LoginDlg(int type = -1, QWidget *parent = nullptr);

signals:
    void sessionCreated(int type, unsigned int sessionId, const QString& sessionName);

private slots:
    void on_btn_cancel_clicked();

    void on_btn_confirm_clicked();

    void onConnectFinished(int type, bool success, unsigned int sessionId);

private:
    void init();

    void setErrorLabel(Error error);

    bool validateInput();

private:
    Ui::Login ui;
    unsigned sessionId = -1;
    QString ip, user, passwd;
    int type;
};
