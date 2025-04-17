#include "logindlg.h"
#include "utils/sessionmanager/sessionmanager.h"
#include <qlistview.h>
#include <qregexp.h>

LoginDlg::LoginDlg(QWidget *parent) : WindowFramelessWidget(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_DeleteOnClose, false);
}

void LoginDlg::init() {
    ui.comboBox->setView(new QListView);
    setErrorLabel(Error::NONE);

    ui.comboBox->setItemData(0, SessionType::SHELL);
    ui.comboBox->setItemData(1, SessionType::SFTP);
    ui.comboBox->setItemData(2, SessionType::SCP);
}

void LoginDlg::setErrorLabel(LoginDlg::Error error) {
    if(error == NONE) {
        ui.widget_error->setVisible(false);
        setFixedSize(388, 346);
        return;
    } else if(error == IP_WRONG) {
        ui.label_error->setText("IP地址错误");
    } else if(error == Error::AUTH_WRONG){
        ui.label_error->setText("用户名或密码验证失败");
    } else if(error == IP_NONE) {
        ui.label_error->setText("IP地址不可为空");
    } else if(error == USER_NONE) {
        ui.label_error->setText("账户不可为空");
    } else if(error == PASSWD_NONE) {
        ui.label_error->setText("登录密码不可为空");
    }
    ui.widget_error->setVisible(true);
    setFixedSize(388, 378);
}

void LoginDlg::on_btn_cancel_clicked() {
    if(sessionId != -1) {
        SMPTR->closeSession(sessionId);
    }
    reject();
}

bool LoginDlg::validateInput() {
    QRegExp ipRegExp("[1-255].[0-255].[0-255].[0-255]");
    ip = ui.edit_ip->text().trimmed();
    user = ui.edit_user->text().trimmed();
    passwd = ui.edit_passwd->text().trimmed();
    if(ip.isEmpty()) {
        setErrorLabel(IP_NONE);
        return false;
    } else if(!ipRegExp.exactMatch(ip)){
        setErrorLabel(IP_WRONG);
        return false;
    }
    if(user.isEmpty()) {
        setErrorLabel(USER_NONE);
        return false;
    }
    if(passwd.isEmpty()) {
        setErrorLabel(PASSWD_NONE);
        return false;
    }
    return true;
}

void LoginDlg::on_btn_confirm_clicked() {
    if(validateInput()) {
//        SMPTR->createSession(ui.comboBox->currentData().value<SessionType>(), user, passwd, ip);
    }
}

