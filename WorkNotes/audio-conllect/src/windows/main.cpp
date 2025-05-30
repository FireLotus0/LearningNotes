#include <qapplication.h>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <qlogcollector.h>
#include <styledstring.h>
#include <crashlistener.h>
#include <qicon.h>
#include <singleinstance/singleinstancecontrol.h>

#include "app.h"
#include "utils/appsettings.h"

#ifndef QT_NO_DEBUG
#include <viewdebug/objectfindtool.h>
#include <viewdebug/filewatcher.h>
#endif

#include <qdebug.h>

int main(int argc, char* argv[]) {
#ifndef QT_NO_DEBUG
    ObjectFinderApplication a(argc, argv);
#else
    QApplication a(argc, argv);
#endif

#ifdef Q_OS_WIN
    /**
     * 设置系统默认字体
    */
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(ncm);
    HRESULT hr;
    hr = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    if (hr != 0) {
        auto font = QApplication::font();
        font.setFamily(QString::fromLocal8Bit(ncm.lfMenuFont.lfFaceName));
        QApplication::setFont(font);
    }
#endif

    /**
     * 初始化日志收集器
     */
#ifdef Q_OS_WIN
    logcollector::styleConfig
        .consoleApp()
        .ide_clion(false)
        .wordWrap(120)
    ;
#elif defined Q_OS_LINUX
    logcollector::styleConfig
            .wordWrap(120)
            .simpleCodeLine();
#endif
    logcollector::QLogCollector::instance().registerLog();

    /**
     * 单例程序初始化
     */
    SingleInstanceControl singleInstanceControl;
    int res = singleInstanceControl.listen();
    if (res != 1) {
        if (res == -1) {
            qDebug() << "cannot start application by instance already created!";
        } else if (res == 0) {
            qCritical() << "error! cannot create instance listener!";
        }
        return 0;
    }

    /**
     * 初始化bugreport，注意填写key
     */
#ifdef QT_NO_DEBUG
    CrashListener crashListener("");
    crashListener.setLogSaveCallback([] (const QString& logFilePath) {
        QFile logFile(logFilePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            logcollector::QLogCollector::save(&logFile);
            logFile.close();
        }
    });
#endif

    /**
     * 打印应用版本等信息
     */
    qDebug() << styled("------application version:").g() << styled(APP_VERSION, true).g() << styled("------").g();

    /**
     * 应用程序初始化在此处开始
     */
    a.setWindowIcon(QIcon(":/res/logo.ico"));

    AppSetting::initProperties();
    if (AppSetting::General::lastSaveDir().isEmpty()) {
        AppSetting::General::lastSaveDir = QCoreApplication::applicationDirPath();
    }
    App app;
    app.show();

    /**
     * 监听到其他应用程序启动
     */
    QObject::connect(&singleInstanceControl, &SingleInstanceControl::anotherInstanceCreated, [&] {
        qInfo() << "block another application start!";
        if (app.isMinimized()) {
            app.showNormal();
        } else if (app.isHidden()) {
            app.show();
        }
    });

    return a.exec();
}