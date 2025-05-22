
#include <qcoreapplication.h>
#include "include/encryptor.h"
#include "src/utils/logger/logger.h"

class Test : public QObject {
    Q_OBJECT
public:
    Test(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void onReadKeyFinished(const QString& key, int errCode) {
        LOG_INFO("Test Read Key Async Finished:", key, " errCode: ", errCode);
    }

    void onReadIdFinished(const QString& id, int errCode) {
        LOG_INFO("Test Read Id Async Finished:" , id, "ErrCode:", errCode);
    }

    void onRefreshKeyFinished(const QString& key, int errCode) {
        LOG_INFO("Test Refresh Key Async Finished:", key, "errCode:" ,errCode);
    }

    void onLoginFinished(bool success, int errCode) {
        LOG_INFO("Login Server Finished: Result=", success, "errCode:" ,errCode);
    }

};

void testRefreshKeySync() {
    auto& manager = ET::KeyManager::instance();;

    char genKen[128];
    memset(genKen, 0, 128);
    std::string sk = "765478";

    auto res = manager.refreshKey((const char*)sk.data(), genKen);
    auto resKey = QString::fromUtf8(genKen, 128);
    LOG_INFO("Test Refresh Key Sync:", resKey, "ErrCode:", res, "RawData:", genKen);
}

void testGetKeySync() {
    auto& manager = ET::KeyManager::instance();;
    char genKen[128];
    memset(genKen, 0, 128);
    auto res = manager.getKey(genKen);
    auto resKey = QString::fromUtf8(genKen, 128);
    LOG_INFO("Test Read Key Sync:", resKey, "ErrCode:", res, "RawData:", genKen);
}

void testGetIdSync() {
    auto& manager = ET::KeyManager::instance();;
    char genKen[128];
    memset(genKen, 0, 128);
    auto res = manager.getId(genKen);
    auto resKey = QString::fromLatin1(genKen, 128).trimmed();
    LOG_INFO("Test Read Id Sync:", resKey, "ErrCode:", res);
}

void testRefreshKeyASync() {
    auto device = ET::UsbManager::instance().getDevice({PID, VID});
    device->generateKeyNonBlock(765478);
}

void testGetKeyAsync() {
    auto device = ET::UsbManager::instance().getDevice({PID, VID});
    device->getKeyNonBlock();
}

void testGetIdAsync() {
    auto device = ET::UsbManager::instance().getDevice({PID, VID});
    device->getIdNonBlock();}

void testGetHostInfo() {
    LOG_INFO("HostInfo: CPU ID =", ET::HostInfo::getCpuId(), " MAC =", ET::HostInfo::getMac());
}

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);
    Test test;
    auto& manager = ET::KeyManager::instance();
    QObject::connect(&manager, &ET::KeyManager::errorOccurred, [&](const QString& err, int code) {
        qDebug() << "Key Manager Error: " << err << " Code:" << code;
    });

    QObject::connect(&manager, &ET::KeyManager::readIdFinished, &test, &Test::onReadIdFinished);
    QObject::connect(&manager, &ET::KeyManager::readKeyFinished, &test, &Test::onReadKeyFinished);
    QObject::connect(&manager, &ET::KeyManager::refreshKeyFinished, &test, &Test::onRefreshKeyFinished);
    QObject::connect(&manager, &ET::KeyManager::loginFinished, &test, &Test::onLoginFinished);

//    QTimer::singleShot(2000, [&]{
//        ET::KeyManager::instance().login("admin", "root");
//    });


//    QTimer::singleShot(2000, [&]{
//        testRefreshKeyASync();
//        testGetKeyAsync();
//        testGetIdAsync();
//    });
//
    QTimer::singleShot(2000, [&]{
        testRefreshKeySync();
        testGetKeySync();
        testGetIdSync();
    });
//    QTimer::singleShot(3000, [&]{
//        testGetKeySync();
//    });
//    QTimer::singleShot(4000, [&]{
//        testGetIdSync();
//    });
//    QTimer::singleShot(5000, [&]{
//        testRefreshKeyASync();
//    });
//    QTimer::singleShot(6000, [&]{
//        testGetKeyAsync();
//    });
//    QTimer::singleShot(7000, [&]{
//        testGetIdAsync();
//    });

    testGetHostInfo();
    return a.exec();
}

#include "main.moc"