#pragma once

#include <QDialog>
#include <qtcpsocket.h>
#include <qmessagebox.h>

#include "ui_connecttoserverdlg.h"

class ConnectToServerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectToServerDlg(QTcpSocket *socket, QWidget *parent = nullptr);

    void promptConnectFail(const QString &errorReason);

signals:
    void connectToServer(bool isContinue);

private slots:
    void on_btn_cancel_clicked();

    void on_btn_connect_clicked();

private:
    Ui::ConnectToServerDlg ui;
    QTcpSocket *socket; //通信
};
