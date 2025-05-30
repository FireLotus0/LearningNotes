#include "filewatcher.h"

#include <QDebug>
#include <QString>
#include <QDir>

FileWatcher::FileWatcher(const std::function<void(const QString& fileName)>& callback)
{
    connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::directoryChanged);
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::fileChanged);

    timer.setInterval(100);
    timer.setSingleShot(true);

    connect(&timer, &QTimer::timeout, [=] {
        QFileInfo file(lastChangedPath);
        callback(file.fileName());
    });
}

void FileWatcher::addPaths(const QString& path, bool recurve)
{
    QStringList listenDir;
    listenDir << path;
    listenDir << entrySubDirAndTargetFileSuffix(path, recurve);

    watcher.addPaths(listenDir);
}

QStringList FileWatcher::entrySubDirAndTargetFileSuffix(const QString& pathSrc, bool recurve) {
    QStringList paths;

    auto dir = QDir(pathSrc);
    auto entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files);
    for (const auto& info: entries) {
        auto path = info.absoluteFilePath();
        if (info.isFile()) {
            if (info.suffix() == fileSuffix) {
                paths << path;
            }
        } else if (info.isDir()) {
            paths << path;
            if (recurve) {
                paths << entrySubDirAndTargetFileSuffix(path, recurve);
            }
        }
    }

    return paths;
}

void FileWatcher::setDirectory(const QString& path, const QString& suffix)
{
    fileSuffix = suffix;
    addPaths(path, true);
}

void FileWatcher::directoryChanged(const QString &path)
{
    Q_UNUSED(path);
    addPaths(path);
}

void FileWatcher::fileChanged(const QString &path)
{
    Q_UNUSED(path);
    timer.start();
    lastChangedPath = path;
}
