#pragma once

#include "ui_app.h"
#include "utils/sessionmanager/sessionmanager.h"
#include "src/widgets/frame/windowframelesshelper.h"

#include <qwidget.h>
#include <qvector.h>

class App : public WindowFramelessWidget<QWidget> {
    Q_OBJECT
public:
    explicit App(QWidget *parent = nullptr);

private slots:
    void on_btn_quit_clicked();

    void on_btn_new_connect_clicked();
protected:
    void showEvent(QShowEvent *event) override;
private:
    void initUi();

private:
    Ui::App ui;
    QVector<unsigned> scpSessions, cmdSessions;
};
