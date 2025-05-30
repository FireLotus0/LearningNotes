#pragma once

#include <qwidget.h>
#include <qtcpsocket.h>
#include <qprogressdialog.h>
#include <qfile.h>

#include "ui_app.h"
#include "dialog/connecttoserverdlg.h"
#include "../modules/codecengine/customcodeengine.h"
#include <qtimer.h>

class App : public QWidget
{
    Q_OBJECT

public:
    explicit App(QWidget *parent = nullptr);
    ~App() override;

private slots:
    void on_btn_set_clicked();

    void on_btn_path_clicked();

    void on_btn_start_clicked();

private:
    void initUsb();

    void monitorTcpComm();

    void receiveClientInfo();

    void sendClientInfo(HostData p);

    void startGather(bool isSet, bool ifStartGather = false);

    void saveDataToBin(const QByteArray& data, int channel);

    void callbackFunc(const FrameData& data);

private:
    Ui::App ui;
    QTcpSocket *socket = nullptr; //通信
    QTimer readUsb;
    QSharedPointer<ConnectToServerDlg> dlg;
    QFile *saveFile = nullptr;
};
