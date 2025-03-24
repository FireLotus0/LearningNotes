#include "app.h"
#include <QGraphicsDropShadowEffect>
#include <QStyleOption>
#include <qpainter.h>

App::App(QWidget *parent)
        : WindowFramelessWidget<QWidget>(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_TranslucentBackground, true);
    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(0x202020);
    shadow->setBlurRadius(10);
    this->setGraphicsEffect(shadow);
    this->setContentsMargins(8, 8, 8, 8);
}

void App::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
