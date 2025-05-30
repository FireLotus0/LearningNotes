#pragma once

#include <QUiLoader>
#include <qboxlayout.h>

#define UI_HOT_LOAD

class HotLoadUiTool {
public:
    HotLoadUiTool();

    static QString getProjectDir();

    virtual void reloadUi() = 0;

    virtual QString targetUiFileName() = 0;

    void loadWidget(QWidget* parent);

private:
    QString checkChildDirect(const QString& path);

private:
    QVBoxLayout* layout;
};