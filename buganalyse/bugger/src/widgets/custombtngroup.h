#pragma once

#include <qwidget.h>

class CustomBtnGroup : public QWidget {
    Q_OBJECT
public:
    enum Orientation {
        HORIZONTAL,
        VERTICAL,
    };
public:
    explicit CustomBtnGroup(QWidget *parent = nullptr);

    void setTagsTexts(const QStringList& texts);

    void setTagsIcons(const QStringList& paths);

    void setTagSize(int width, int height);

    void setTagRadius(int radius);

    void setPadding(int lPadding, int rPadding);

    void setRoundPos(Orientation pos);

    void setBackColor(const QColor& colors);

    void setSelectColor(const QColor& color);

    void setTagFont(const QFont& font);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<QPair<QString, QPixmap>> btnRes;
    int tagRadius = -1;
    int curSelected = -1;
    int leftPadding, upPadding;
    int tagDistance = 0;
    int tagWidth, tagHeight;
    Orientation roundPos;
    QColor backColor;
    QColor selectedColor;
    QFont tagFont;
    QPainter* painter{nullptr};
};
