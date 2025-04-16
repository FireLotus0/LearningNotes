#pragma once

#include "src/widgets/frame/windowframelesshelper.h"
#include "ui_login.h"
#include <qdialog.h>


class LoginDlg : public WindowFramelessWidget<QDialog> {
    Q_OBJECT
public:
    explicit LoginDlg(QWidget *parent = nullptr);



private:
    void init();

private:
    Ui::Login ui;
};
