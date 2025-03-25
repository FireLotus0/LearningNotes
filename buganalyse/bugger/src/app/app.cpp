#include "app.h"
#include <QGraphicsDropShadowEffect>
#include <QStyleOption>
#include <qpainter.h>
#include <qdebug.h>
App::App(QWidget *parent)
        : WindowFramelessWidget<QWidget>(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
}

void App::initUi() {
    ui.vTag->init(CustomBtnGroup::VERTICAL, QColor("#f7f6e7"), QColor("#c2e9fb"), {":/res/res/session.png", ":/res/res/search_dev.png", ":/res/res/setting.png"}, true);
}

void App::showEvent(QShowEvent *event) {
    initUi();
    QWidget::showEvent(event);
}

void App::on_btn_quit_clicked() {
    this->close();
}
