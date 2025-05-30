#include "utils/transfermanager.h"
#include <QFile>
#include <qcoreapplication.h>

#include "crashlistener.h"
#include "utils/configadc.h"
#include <qlogcollector.h>

int main(int argc, char *argv[]) {

    /**
 * 初始化bugreport，注意填写key
 */
#ifdef QT_NO_DEBUG
    CrashListener crashListener(ACCESS_KEY);
    crashListener.setLogSaveCallback([](const QString &logFilePath) {
        QFile logFile(logFilePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            logcollector::QLogCollector::save(&logFile);
            logFile.close();
        }
    });
#endif

    ad4630_reg_write(0x0099);// reset

    // chip test
    auto rev = ad4630_reg_read(0x8300);
    ad4630_reg_write(0x0A5A);
    rev = ad4630_reg_read(0x8A00);

    ad4630_reg_write(0x02D1);// HP mode
    rev = ad4630_reg_read(0x8200);

    ad4630_reg_write(0x1120);// 24-bit
    ad4630_reg_write(0x1202);// four-lane
    rev = ad4630_reg_read(0x9500);
    ad4630_reg_write(0x1EAA);// sinc3

    rev = ad4630_reg_write(0x2528);
    rev = ad4630_reg_read(0xA500);

    ad4630_reg_write(0x0182);// reset


    QCoreApplication a(argc, argv);

    TransferManager::instance().startServer();

    return a.exec();
}