#pragma once

#include "ui_app.h"
#include "src/utils/frame/windowframelesshelper.h"
#include <qwidget.h>

class App : public WindowFramelessWidget<QWidget> {
    Q_OBJECT
public:
    explicit App(QWidget *parent = nullptr);

private slots:
    void on_btn_quit_clicked();

protected:
    void showEvent(QShowEvent *event) override;
private:
    void initUi();

private:
    Ui::App ui;
};
