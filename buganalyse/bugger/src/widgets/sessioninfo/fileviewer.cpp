
#include "fileviewer.h"
#include <qtreeview.h>
#include <qscrollbar.h>

FileViewer::FileViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUi();
    initSigSlots();
}

void FileViewer::initUi() {
    auto tree = ui.treeView;
    tree->horizontalScrollBar()->setVisible(false);     // 通过splitter代替水平滚动条
    tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  // 垂直方向根据像素滚动
    tree->setIconSize(QSize(24, 24));
    tree->setSelectionBehavior(QAbstractItemView::SelectItems); // 选择Item
    tree->setSelectionMode(QAbstractItemView::SingleSelection);   // 单选
    tree->setDragEnabled(false);        // 不可拖动
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);   // 不可编辑
}

void FileViewer::onRightBtnClicked(const QModelIndex& index) {
    if(QGuiApplication::mouseButtons().testFlag(Qt::RightButton)) {

    }
}

void FileViewer::onItemClicked(const QModelIndex &index) {

}

void FileViewer::initSigSlots() {
    connect(ui.treeView, &QTreeView::clicked, this, &FileViewer::onItemClicked);
    connect(ui.treeView, &QTreeView::pressed, this, &FileViewer::onRightBtnClicked);
}

