#include "logindlg.h"
#include "utils/sessionmanager/sessionmanager.h"
#include <qlistview.h>
#include <qregexp.h>

LoginDlg::LoginDlg(int type, QWidget *parent)
    : WindowFramelessWidget(parent)
    , type(type)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_DeleteOnClose, false);
    init();
}

void LoginDlg::init() {
    ui.comboBox->setView(new QListView);
    setErrorLabel(Error::NONE);
    ui.comboBox->setItemData(0, (int)SessionType::SHELL);
    ui.comboBox->setItemData(1, (int)SessionType::SFTP);
    ui.comboBox->setItemData(2, (int)SessionType::SCP);

    connect(SMPTR, &SessionManager::sigSessionConnectFinished, this, &LoginDlg::onConnectFinished);
    auto edit = findChildren<QLineEdit*>();
    for(auto e : edit) {
        connect(e, &QLineEdit::textEdited, this, [&](const QString&){ setErrorLabel(NONE); });
    }
    if(type != -1) {
        if(type == SessionType::SHELL) {
            ui.comboBox->setCurrentIndex(0);
        } else if(type == SessionType::SFTP) {
            ui.comboBox->setCurrentIndex(1);
        } else if(type == SessionType::SCP) {
            ui.comboBox->setCurrentIndex(2);
        } else {
            Q_ASSERT(false);
        }
        ui.widget_type->setVisible(false);
    }
}

void LoginDlg::setErrorLabel(LoginDlg::Error error) {
    if(error == NONE) {
        ui.label_error->setVisible(false);
        ui.icon_error->setVisible(false);
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
    ui.label_error->setVisible(true);
    ui.icon_error->setVisible(true);
}

void LoginDlg::on_btn_cancel_clicked() {
    if(sessionId != -1) {
        SMPTR->closeSession(sessionId);
    }
    reject();
}

bool LoginDlg::validateInput() {
    QRegExp ipRegExp(R"(^((2((5[0-5])|([0-4]\d)))|([0-1]?\d{1,2}))(\.((2((5[0-5])|([0-4]\d)))|([0-1]?\d{1,2}))){3}$)");
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
        SMPTR->createSession((SessionType)ui.comboBox->currentData().toInt(), user, passwd, ip);
    }
}

void LoginDlg::onConnectFinished(int type, bool success, unsigned int sessionId) {
    if(success) {
        emit sessionCreated(type, sessionId, user + "@" + ip);
        close();
    } else {
        setErrorLabel(Error::AUTH_WRONG);
    }
}

