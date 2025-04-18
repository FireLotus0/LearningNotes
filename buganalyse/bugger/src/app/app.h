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

    void on_btn_ssh_clicked();

    void onNewConnCreated(int type, unsigned int id, const QString& sessionName);
protected:
    void showEvent(QShowEvent *event) override;
private:
    void initUi();

private:
    Ui::App ui;
    QVector<unsigned> scpSessions, cmdSessions;
};
