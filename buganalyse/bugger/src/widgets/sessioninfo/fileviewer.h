#pragma once

#include "ui_fileviewer.h"
#include <qwidget.h>

class FileViewer : public QWidget {
    Q_OBJECT
public:
    FileViewer(QWidget* parent = nullptr);

private slots:
    /// 鼠标右键点击
    void onRightBtnClicked(const QModelIndex& index);

    /// 点击item
    void onItemClicked(const QModelIndex& index);
private:
    void initUi();

    void initSigSlots();
private:
    Ui::FileViewer ui;
};


