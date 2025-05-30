#pragma once

#include <functional>
#include <QFileSystemWatcher>
#include <QTimer>

class FileWatcher : public QObject
{
public:
    FileWatcher(const std::function<void(const QString& fileName)>& callback);

    void setDirectory(const QString &path, const QString& suffix);

public:
    void directoryChanged(const QString &path);
    void fileChanged(const QString &path);

private:
    void addPaths(const QString& path, bool recurve = false);
    QStringList entrySubDirAndTargetFileSuffix(const QString& path, bool recurve);

private:
    QFileSystemWatcher watcher;
    QTimer timer;
    QString lastChangedPath;
    QString fileSuffix;
};
