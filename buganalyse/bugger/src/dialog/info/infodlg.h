#pragma once

#include "src/widgets/frame/windowframelesshelper.h"
#include "ui_InfoDlg.h"
#include <qdialog.h>
#include <QQueue>

class InfoDlg : public WindowFramelessWidget<QDialog>
{
    Q_OBJECT
public:
    explicit InfoDlg(QWidget *parent = nullptr);

    static InfoDlg* getInstance();

    void setStateStr(bool error, const QString& str);

private slots:
    void on_btn_close_clicked();

private:
    Ui::InfoDlg ui;
    QQueue<QPair<bool, QString>> stateCache;
};
