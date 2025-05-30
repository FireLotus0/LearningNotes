#include "connecttoserverdlg.h"

#include <qregularexpression.h>
#include <qhostaddress.h>

ConnectToServerDlg::ConnectToServerDlg(QTcpSocket *socket, QWidget *parent)
    : QDialog(parent)
    , socket(socket)
{
    ui.setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint);
    ui.lineEdit_adress->setText("192.168.1.233");
    ui.lineEdit_port->setText("12345");
}

void ConnectToServerDlg::on_btn_cancel_clicked() {
    emit connectToServer(false);
    hide();
}

void ConnectToServerDlg::on_btn_connect_clicked() {
    QRegularExpression re("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    if (!re.match(ui.lineEdit_adress->text()).hasMatch()) {
        QMessageBox::warning(this, "Error", "Invalid IP address");
        return;
    }
    socket->connectToHost(QHostAddress(ui.lineEdit_adress->text()), ui.lineEdit_port->text().toInt());
}

void ConnectToServerDlg::promptConnectFail(const QString &errorReason) {
    QMessageBox::warning(this, "Error", "Connection failed, reason: " + errorReason);
}
