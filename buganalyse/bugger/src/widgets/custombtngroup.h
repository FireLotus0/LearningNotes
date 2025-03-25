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

    void init(Orientation orientation, const QColor &bkColor, const QColor &selected, const QStringList &texts,
              bool isPixmap);

    void setTagsRes(const QStringList &texts, bool isPixmap);

    void setTagsIcons(const QStringList &paths);

    void setTagSize(int width = 0, int height = 0);

    void setTagRadius(int radius);

    void setPadding(int lPadding, int uPadding);

    void setRoundPos(Orientation pos);

    void setBackColor(const QColor &colors);

    void setSelectColor(const QColor &color);

    void setTagFont(const QFont &font);

    void setIconPadding(int padding);

    void setBorderRadius(int leftUp, int leftBottom, int rightUp, int rightBottom);
signals:
    void indexChanged(int index);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    QList<QPair<QString, QPixmap>> tagRes;
    int tagRadius = 0;
    int curSelected = 0;
    int leftPadding = 0, upPadding = 0;
    int tagDistance = 0;
    int tagWidth, tagHeight;
    Orientation roundPos;
    QColor backColor;
    QColor selectedColor;
    QFont tagFont;
    int iconPadding = 4;
    int leftUpRadius = 0, rightUpRadius = 0, leftBomRadius = 0, rightBomRadius = 0;
};
