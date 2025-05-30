#pragma once

#include "observerproperty.h"

#include <qsettings.h>
#include <qstandardpaths.h>
#include <qdir.h>
#include <qcoreapplication.h>

template<typename T, char const* group>
class IniProperty : public ObserverProperty<T> {
public:
    using ObserverProperty<T>::ObserverProperty;
    using ObserverProperty<T>::operator=;

protected:
    void saveData(const QString& key, const T& value) override {
        QSettings settings(getSavedFilePath(), QSettings::IniFormat);
        settings.beginGroup(group);
        settings.setValue(key, value);
        settings.endGroup();
    }

    T readData(const QString& key, const T& defaultValue) override {
        QSettings settings(getSavedFilePath(), QSettings::IniFormat);
        settings.beginGroup(group);
        auto data = settings.value(key, defaultValue);
        settings.endGroup();
        return data.template value<T>();
    }

    QString getSavedFilePath() {
#ifdef __GNUC__
        static auto filePath = QCoreApplication::applicationDirPath();
#else
        static auto filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
        QDir dir(filePath);
        if (!dir.exists()) {
            dir.mkdir(filePath);
        }
        return filePath + "/" + iniFileName();
    }

    virtual QString iniFileName() = 0;
};