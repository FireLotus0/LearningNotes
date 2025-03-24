#include "custombtngroup.h"
#include <qpainter.h>

CustomBtnGroup::CustomBtnGroup(QWidget *parent)
        : QWidget(parent)
{
}

void CustomBtnGroup::setTagFont(const QFont &font) {

}

void CustomBtnGroup::paintEvent(QPaintEvent *event) {
    if(!painter) {
        painter = new QPainter(this);
    }
    painter->fillRect(rect(), QBrush(backColor));
    QPainter path;
    if(roundPos == Orientation::HORIZONTAL) {
        int startX = 0, startY = QWidget::height() - tagHeight;
        if(curSelected != 0) {
            startX = leftPadding + (curSelected + 1) * (tagWidth + tagDistance) - 2 * tagRadius;
        }
        path.drawArc(startX, startY, 2 * tagRadius, 2 * tagRadius, 270, 90);
        path.drawLine(startX + 2 * tagRadius, startY + tagHeight, startX + 2 * tagRadius, startY + tagRadius);
        path.drawArc(startX + 2 * tagRadius, startY, 2 * tagRadius, 2 * tagRadius, 180, -90);
        path.drawLine(startX + 3 * tagRadius, startY, startX + tagRadius + tagWidth, startY);
        path.drawArc(startX + 2 * tagRadius, startY, 2 * tagRadius, 2 * tagRadius, 180, -90);


    }
}

void CustomBtnGroup::setTagSize(int width, int height) {
//    Q_ASSERT(height > tagRadius + ())
}
