#pragma once
#include "iomanager.h"
#include "protocolcodecengine.h"
#include "src/bean/commomtype.h"
#include "src/utils/aes/encry_aes.h"
#include "src/utils/hostinfo/hostinfo.h"
#include "src/utils/logger/logger.h"
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qqueue.h>
#ifdef OS_LINUX
#include <libusb-1.0/libusb.h>
#else
#include "libusb.h"
#endif

PROTOCOL_CODEC_USING_NAMESPACE

namespace ET {
#define PID 0x4831
#define VID 0x4831
#define CTL_POINT 1

class BlockIoContext{
private:
    QString& result;
public:
    BlockIoContext(QString& resRef, QObject* parent = nullptr) : result(resRef){}

    void onRefreshKeyFinished(const GenKeyFd& data) {
        result = data.key;
    }

    void onGetKeyFinished(const ReadKeyFd& data) {
        result = data.key;
    }

    void onReadIdFinished(const ReadIdFd& data) {
        result = data.id;
    }
};

class UsbDevice : public QObject
{
Q_OBJECT
public:
    enum OPERATION
    {
        OPEN_DEVICE = 1,
        DETACH_KERNEL = 1 << 1,
        SET_CONFIGURATION = 1 << 2,
        CLAIM_INTERFACE = 1 << 3,
        REGISTER_CALLBACK = 1 << 4,
    };
    Q_ENUM(OPERATION)

public:
    explicit UsbDevice(libusb_device *device, libusb_context *ctx = nullptr, uint16_t productId = LIBUSB_HOTPLUG_MATCH_ANY,
                       uint16_t vendorId = LIBUSB_HOTPLUG_MATCH_ANY, uint16_t classId = LIBUSB_HOTPLUG_MATCH_ANY, QObject *parent = nullptr);

    ~UsbDevice() override;

    void openDevice();

    bool isOpened() const;

    void insertEndPoint(uint8_t pointNumer, libusb_transfer_type transferType, IOType type);

    libusb_device_handle* getDeviceHandle();

    bool interfaceRequired(int configIndex, int interfaceIndex) const;

    void getKeyNonBlock();

    void getIdNonBlock();

    void generateKeyNonBlock(unsigned long sk);

    /// 阻塞接口
    QString refreshKeyBlocked(unsigned long sk);

    QString getKeyBlocked();

    QString getIdBlocked();

public slots:
    void onTransferFinished(IOType ioType, QByteArray data, int cmdType);

    void onErrorOccurred(const QString& errStr, int errCode);

    void onTimeOut(IOType ioType);

signals:
    // 同步控制传输
    void doSyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data, int cmdType);
    // 同步批量传输
    void doSyncBulkIO(unsigned char endpoint, int length, IOType ioType, const QByteArray& data, int cmdType);
    // 同步中断传输
    void doSyncInterIo(unsigned char endpoint,  IOType type, int cmdType, unsigned long sk);
    // 异步控制传输
    void doAsyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray &data = QByteArray());
    // 异步中断传输
    void doAsyncInterIO(uint8_t endPoint, IOType ioType, const QByteArray& data = QByteArray());
    // 异步等时传输
    void doAsyncIsoIO(uint8_t endPoint, IOType ioType, uint8_t  pointNum, const QByteArray& data = QByteArray());

    void information(const  QString& errorStr, int errCode);

    void readKeyFinished(const QString& key);

    void readIdFinished(const QString& id);

    void generateKeyFinished(const QString& key);

private:
    /**
     * @brief 检查执行结果并打印错误
     * @param res
     * @param op
     * @return
     */
    bool checkOperationRes(int res, OPERATION op);

    void registerCodeC();

    void insertIntoCache(int cmd, const QByteArray& data);

    void writeNextCommand();
    /*!
     * @param ioType 0 Read Key 1 Refresh Key 2 Read Id
     * @return
     */
    QString doIoBlocked(int ioType, unsigned long sk = 0);

    void doIoNonBlock(int cmdType, unsigned long sk = 0, uint8_t endPoint = 1);

    void detachKernel();
private:
    uint16_t productId, vendorId, classId;
    int usedConfigNum = 0, usedInterfaceNum = 0;
    QMap<uint8_t, IoManager::ThrType> pointTransType;
    int devValid = 0;
    libusb_device_handle *deviceHandle{nullptr};
    libusb_device *device;
    libusb_context *ctx;
    QString identifier{};
    bool isIoBusy = false;
    QQueue<QPair<int, unsigned long>> cmdCache;
    bool stateError = true;
    QString key, id, cpuId, mac;
};
}