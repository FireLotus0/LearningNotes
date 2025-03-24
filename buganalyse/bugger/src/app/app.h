#pragma once

#include "ui_app.h"
#include "src/utils/frame/windowframelesshelper.h"
#include <qwidget.h>

class App : public WindowFramelessWidget<QWidget> {
public:
    explicit App(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::App ui;
};
