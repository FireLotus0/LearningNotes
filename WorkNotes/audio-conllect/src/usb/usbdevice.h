#pragma once
#include "iomanager.h"
#include "libusb.h"
#include "protocolcodecengine.h"
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qqueue.h>

PROTOCOL_CODEC_USING_NAMESPACE

#define PID 0x0104
#define VID 0x1d6b
#define DATA_POINT 1

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

    void read(uint8_t point);

    void write(const QByteArray& data);

signals:
    void information(const  QString& errorStr, bool isError);

    void readFinished(const QByteArray& data);

public slots:
    void onTransferFinished(IOType ioType, QByteArray data);

    void onErrorOccurred(const QString& errStr);

    void onTimeOut(IOType ioType);

signals:
    // 同步控制传输
    void doSyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data);
    // 同步批量传输
    void doSyncBulkIO(unsigned char endpoint, int length, IOType ioType, const QByteArray& data);
    // 同步中断传输
    void doSyncInterIo(unsigned char endpoint, int length, IOType ioType, const QByteArray& data);
    // 异步控制传输
    void doAsyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray &data = QByteArray());
    // 异步中断传输
    void doAsyncInterIO(uint8_t endPoint, IOType ioType, const QByteArray& data = QByteArray());
    // 异步等时传输
    void doAsyncIsoIO(uint8_t endPoint, IOType ioType, uint8_t  pointNum, const QByteArray& data = QByteArray());


private:
    /**
     * @brief 检查执行结果并打印错误
     * @param res
     * @param op
     * @return
     */
    bool checkOperationRes(int res, OPERATION op);

    void insertIntoCache(int cmd, const QByteArray& data);

    void writeNextCommand();

    void transfer(const QByteArray& data, uint8_t point);
private:
    uint16_t productId, vendorId, classId;
    int usedConfigNum = 0, usedInterfaceNum = 0;
    QMap<uint8_t, IoManager::ThrType> pointTransType;
    int devValid = 0;
    libusb_device_handle *deviceHandle{nullptr};
    libusb_device *device;
    libusb_context *ctx;
    QString identifier{};
    bool isWriting = false;
    QQueue<QByteArray> writeCmdCache;
    bool stateError = true;
};
