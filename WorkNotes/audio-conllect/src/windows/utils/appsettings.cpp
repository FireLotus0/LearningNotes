#include "appsettings.h"

namespace AppSetting {

    char const General::g[] = "general";
    AppSettings<QString, General::g> General::lastSaveDir("lastSaveDir");

    void initProperties() {
        General::lastSaveDir.init();
    }
}