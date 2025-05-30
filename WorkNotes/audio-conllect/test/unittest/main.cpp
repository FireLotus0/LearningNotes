#include <qcoreapplication.h>
#include <qtest.h>

#include "tester/unittest.h"

#include <qlogcollector.h>
#include <styledstring.h>

#include <qfile.h>
#include <qtextstream.h>

void printOutput() {
    QFile data("test.txt");
    if (!data.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&data);
    while (!in.atEnd()) {
        auto log = in.readLine();
        if (log.startsWith("PASS")) {
            qInfo() << styled(log).g();
        } else if (log.startsWith("FAIL")) {
            qCritical() << styled(log).r();
        } else if (log.startsWith("Totals")) {
            qInfo() << styled(log).y(1);
        } else if (log.startsWith("QWARN")) {
            qWarning() << styled(log).y();
        } else {
            qDebug().noquote() << log;
        }
    }
    data.close();
}

template<typename... Arg> struct TestRunner;
template<typename T, typename... Arg>
struct TestRunner<T, Arg...> : TestRunner<Arg...> {
    static int run() {
        T t;
        int result = QTest::qExec(&t, {"unit-test.exe", "-o", "test.txt"});
        printOutput();
        result += TestRunner<Arg...>::run();
        return result;
    }
};

template<> struct TestRunner<> {
    static int run() { return 0; }
};

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    /**
     * 初始化日志收集器
     */
#ifdef Q_OS_WIN
    logcollector::styleConfig
            .consoleApp()
            .ide_clion(false)
            .wordWrap(180)
            ;
#elif defined Q_OS_LINUX
    logcollector::styleConfig
            .wordWrap(120)
            .simpleCodeLine();
#endif
    logcollector::QLogCollector::instance().registerLog();

    /**
     * 创建测试
     */
    int result = TestRunner<UnitTest
            >::run();

    if (result != 0) {
        qDebug() << styled("Not all tests are successful!").r();
    } else {
        qDebug() << styled("All tests passed!").g();
    }

    return 0;
}