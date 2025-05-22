#include "hostinfo.h"
#include "src/utils/logger/logger.h"
#include <qfile.h>
#include <qprocess.h>
#include <iostream>
#include <qthread.h>
#include <qregularexpression.h>

namespace ET {
namespace HostInfo {

QString getCpuId() {
#ifdef OS_WIN
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0); // 获取 CPU 信息
    QString cpuId = QString("Vendor ID: %1")
            .arg(QString::fromLocal8Bit(reinterpret_cast<const char*>(&cpuInfo[1]), 4) +
                 QString::fromLocal8Bit(reinterpret_cast<const char*>(&cpuInfo[2]), 4) +
                 QString::fromLocal8Bit(reinterpret_cast<const char*>(&cpuInfo[3]), 4));

    // 获取处理器型号
    __cpuid(cpuInfo, 0x80000002); // 获取处理器型号的前部分
    QString model = QString::fromLocal8Bit(reinterpret_cast<const char*>(cpuInfo), sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000003); // 获取处理器型号的中间部分
    model += QString::fromLocal8Bit(reinterpret_cast<const char*>(cpuInfo), sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000004); // 获取处理器型号的后部分
    model += QString::fromLocal8Bit(reinterpret_cast<const char*>(cpuInfo), sizeof(cpuInfo));

    return model.trimmed();

#elif defined(OS_LINUX)

    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Unable to open /proc/cpuinfo";
    }

    QString fileData = file.readAll();
    // 正则表达式
    QRegularExpression regex(R"(model name\s*:\s*(.*))");
    QRegularExpressionMatchIterator it = regex.globalMatch(fileData);

    // 输出所有匹配到的 model name
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString modelName = match.captured(1); // 捕获组1是 model name 的值
        if(!modelName.isEmpty()) {
            qDebug() << "Model Name:" << modelName.split(":").last();
            return modelName.split(":").last();
        }
    }

    return "xxxxxx";

#else
    return QString("Unsupported OS");
#endif
}

QString getMac() {
    QString macAddress;
    for(const auto& inter : QNetworkInterface::allInterfaces()) {
        if (inter.flags().testFlag(QNetworkInterface::IsUp) &&
                      !inter.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            macAddress = inter.hardwareAddress();
            break; // 获取第一个有效的 MAC 地址
        }
    }
//    return macAddress;
    return "EC:D6:8A:FE:56:D5";
}
}
}