#include "custombtngroup.h"
#include <qpainter.h>
#include <QPainterPath>
#include <QStyleOption>
#include <qdebug.h>
#include <qevent.h>

CustomBtnGroup::CustomBtnGroup(QWidget *parent)
        : QWidget(parent)
{

}

void CustomBtnGroup::setTagFont(const QFont &font) {
    tagFont = font;
}

void CustomBtnGroup::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QBrush(backColor));
    QPainterPath path;
    auto edgeLen = 2 * tagRadius;
    if (roundPos == Orientation::HORIZONTAL) {
        int startY = upPadding;
        int startX = curSelected != 0 ? leftPadding + (curSelected ) * (tagWidth + tagDistance) - edgeLen : leftPadding - edgeLen;
        if (curSelected == 0) {
            path.moveTo(0, height());
            path.lineTo(0, upPadding);
            path.lineTo(leftPadding + tagWidth, upPadding);
        } else {
            path.moveTo(startX + tagRadius, height());
            path.arcTo(startX, height() - edgeLen, edgeLen, edgeLen, 270, 90);
            path.lineTo(startX + edgeLen, startY + tagRadius);
            path.arcTo(startX + edgeLen, startY, edgeLen, edgeLen, 180, -90);
            path.lineTo(startX + tagRadius + tagWidth, startY);
        }
        path.arcTo(startX + tagWidth, startY, edgeLen, edgeLen, 90, -90);
        path.lineTo(startX + edgeLen + tagWidth, height() - tagRadius);
        path.arcTo(startX + edgeLen + tagWidth, height() - edgeLen, edgeLen, edgeLen, 180, 90);
        path.lineTo(startX, height());
        painter.fillPath(path, QBrush(selectedColor));

        for (int i = 0; i < tagRes.size(); i++) {
            auto rect = QRect{leftPadding + i * (tagWidth + tagDistance) + iconPadding, startY + iconPadding, tagWidth - 2 * iconPadding, tagHeight - 2 * iconPadding};
            if (tagRes[i].second.isNull()) {
                painter.drawText(rect, Qt::AlignCenter, tagRes[i].first);
            } else {
                painter.drawPixmap(rect, tagRes[i].second);
            }
        }
    } else {
        int startY = curSelected != 0 ? upPadding + (curSelected ) * (tagHeight + tagDistance) - edgeLen : upPadding - edgeLen;
        if (curSelected == 0) {
            path.moveTo(width(), upPadding);
            path.lineTo(leftPadding, upPadding);
            path.lineTo(leftPadding, upPadding + tagHeight - tagRadius);
        } else {
            path.moveTo(width(), startY - tagRadius);
            path.arcTo(width() - edgeLen, startY, edgeLen, edgeLen, 0, -90);
            path.lineTo(width() - tagRadius, startY + edgeLen);
            path.arcTo(leftPadding, startY + edgeLen, edgeLen, edgeLen, 90, 90);
            path.lineTo(leftPadding, startY + edgeLen + tagHeight - tagRadius);
        }
        path.arcTo(leftPadding, startY + tagHeight, edgeLen, edgeLen, 180, 90);
        path.lineTo(width() - tagRadius, startY + edgeLen + tagHeight);
        path.arcTo(width() - edgeLen, startY + edgeLen + tagHeight, edgeLen, edgeLen, 90, -90);
//        path.lineTo(width(), height());
        painter.fillPath(path, QBrush(selectedColor));

        for (int i = 0; i < tagRes.size(); i++) {
            auto rect = QRect{leftPadding + iconPadding, upPadding + i * (tagHeight + tagDistance) + iconPadding, tagWidth - 2 * iconPadding, tagHeight- 2 * iconPadding};
            if (tagRes[i].second.isNull()) {
                painter.drawText(rect, Qt::AlignCenter, tagRes[i].first);
            } else {
                painter.drawPixmap(rect, tagRes[i].second);
            }
        }
    }
    QWidget::paintEvent(event);
}

void CustomBtnGroup::setTagSize(int width, int height) {
    if (roundPos == Orientation::HORIZONTAL && height == 0) {
        tagWidth = width;
        tagHeight = this->height();
        upPadding = 0;
    } else if (roundPos == Orientation::VERTICAL && width == 0) {
        tagWidth = this->width();
        tagHeight = height;
        leftPadding = 0;
    } else {
        tagWidth = width;
        tagHeight = height;
    }
}

void CustomBtnGroup::setTagsRes(const QStringList &texts, bool isPixmap) {
    if (isPixmap) {
        for (const auto &text: texts) {
            tagRes.append({"", QPixmap(text)});
        }
    } else {
        for (const auto &text: texts) {
            tagRes.append({text, QPixmap()});
        }
    }
}

void CustomBtnGroup::setSelectColor(const QColor &color) {
    selectedColor = color;
}

void CustomBtnGroup::setTagsIcons(const QStringList &paths) {


}

void CustomBtnGroup::setBackColor(const QColor &color) {
    backColor = color;
}

void CustomBtnGroup::setRoundPos(CustomBtnGroup::Orientation pos) {
    roundPos = pos;
}

void CustomBtnGroup::setPadding(int lPadding, int uPadding) {
    leftPadding = lPadding;
    upPadding = uPadding;
}

void CustomBtnGroup::setTagRadius(int radius) {
    tagRadius = radius;
}

void CustomBtnGroup::mousePressEvent(QMouseEvent *event) {
    int pos, index;
    if(roundPos == Orientation::HORIZONTAL) {
        pos = event->pos().x();
        index = pos / (tagWidth + tagDistance);

    } else {
        pos = event->pos().y();
        index = pos / (tagHeight + tagDistance);
    }
    if(index >= 0 && index < tagRes.size()) {
        if(curSelected != index) {
            emit indexChanged(index);
            curSelected = index;
            repaint();
        }
    }
    QWidget::mousePressEvent(event);
}

void CustomBtnGroup::init(CustomBtnGroup::Orientation orientation, const QColor &bkColor, const QColor &selected,
                          const QStringList &texts, bool isPixmap) {
    roundPos = orientation;
    backColor = bkColor;
    selectedColor = selected;
    setTagsRes(texts, isPixmap);
    if(isPixmap && !texts.isEmpty()) {
        tagWidth = tagRes[0].second.width() + 2 * iconPadding;
        tagHeight = tagRes[0].second.height() + 2 * iconPadding;
        tagRadius = 8;
        tagDistance = 8;
    }
    if(orientation == Orientation::HORIZONTAL) {
        leftPadding = 0;
        upPadding = 1;
    } else {
        leftPadding = 1;
        upPadding = 0;
    }
}

void CustomBtnGroup::setIconPadding(int padding) {
    iconPadding = padding;
}

void CustomBtnGroup::setBorderRadius(int leftUp, int leftBottom, int rightUp, int rightBottom) {

}
