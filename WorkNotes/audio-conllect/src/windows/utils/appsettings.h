#pragma once

#include <qobject.h>

#include <listener/iniproperty.h>

namespace AppSetting {
    template<typename T, char const *group>
    class AppSettings : public IniProperty<T, group> {
    public:
        using IniProperty<T, group>::IniProperty;
        using IniProperty<T, group>::operator=;

    protected:
        QString iniFileName() override {
            return "appsettings.ini";
        }
    };

    struct General {
        static char const g[];

        static AppSettings<QString, g> lastSaveDir;
    };

    void initProperties();
}