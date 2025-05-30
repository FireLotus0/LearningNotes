#include "hotloaduitool.h"

#include <qdir.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qwidget.h>

HotLoadUiTool::HotLoadUiTool() : layout(nullptr) {}

QString HotLoadUiTool::getProjectDir() {
    return CUR_PROJECT_PATH;
}

void HotLoadUiTool::loadWidget(QWidget* parent) {
#ifdef QT_DEBUG
    auto fileAbsolutePath = checkChildDirect(getProjectDir());
    if (fileAbsolutePath.isEmpty()) {
        return;
    }

    QUiLoader loader;

    QFile file(fileAbsolutePath);
    file.open(QFile::ReadOnly);
    QWidget* widget = loader.load(&file, parent);
    file.close();

    if (widget != nullptr) {
        parent->resize(widget->geometry().size());
        if (layout == nullptr) {
            layout = new QVBoxLayout(parent);
            layout->setContentsMargins(0, 0, 0, 0);
        }
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        layout->addWidget(widget);
    }
#endif
}

QString HotLoadUiTool::checkChildDirect(const QString& path) {
    auto fileInfos = QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    for (const auto& info : fileInfos) {
        if (info.isFile()) {
            if (info.fileName() == targetUiFileName()) {
                return info.absoluteFilePath();
            }
        } else if (info.isDir()) {
            auto name = checkChildDirect(info.absoluteFilePath());
            if (!name.isEmpty()) {
                return name;
            }
        }
    }
    return QString();
}
