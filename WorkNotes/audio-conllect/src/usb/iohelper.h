#pragma once

#include "libusb.h"
#include <qobject.h>
#include <qtimer.h>
// 缓冲区大小
#define BULK_BUF_SIZE 1024
#define CTL_BUF_SIZE 64
#define INTER_BUF_SIZE 64
// 传输超时
#define IO_TIME_OUT 1500

enum IOType
{
    WRITE = 0,
    READ,
};
Q_DECLARE_METATYPE(IOType)

class AbstractIO : public QObject{
    Q_OBJECT
public:
    AbstractIO(QObject* parent = nullptr);
protected:
    bool checkResult(int res, IOType);
signals:
    void errorOccured(const QString &errStr);
    void finished(IOType ioType, QByteArray data);
    void timeout(IOType);
};

// 同步IO
class SyncIO : public AbstractIO
{
    Q_OBJECT
public:
    explicit SyncIO(libusb_device_handle *deviceHandle, int buffSz = CTL_BUF_SIZE, unsigned int timeout = IO_TIME_OUT, QObject *parent = nullptr);
protected:
    QByteArray rdBuffer;// 除端点0外，其余端点为单向传输
    QByteArray wrBuffer;// 除端点0外，其余端点为单向传输
    libusb_device_handle *deviceHandle;
    unsigned int timeout;
};

class CtlIOSync : public SyncIO
{
    Q_OBJECT
public:
    explicit CtlIOSync(libusb_device_handle *dev_handle, int buffSz = CTL_BUF_SIZE, unsigned int timeout = IO_TIME_OUT, QObject *parent = nullptr);
public slots:
    void doTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data);
private:
    QByteArray readBuffer;
};

class BulkIOSync : public SyncIO
{
    Q_OBJECT
public:
    explicit BulkIOSync(IOType type, libusb_device_handle *dev_handle, int buffSz = BULK_BUF_SIZE, unsigned int timeout = IO_TIME_OUT, QObject *parent = nullptr);
public slots:
    void doTransfer(unsigned char endpoint, int length, IOType ioType, const QByteArray& data);
private:
    IOType ioType;
};

class InterruptIOSync : public SyncIO
{
    Q_OBJECT
public:
    explicit InterruptIOSync(libusb_device_handle *dev_handle, int buffSz = CTL_BUF_SIZE, unsigned int timeout = IO_TIME_OUT, QObject *parent = nullptr);
public slots:
    void doTransfer(unsigned char endpoint, int length, IOType ioType, const QByteArray& data);
};

class AsyncIO;
struct AsyncUserData {
    int length;
    IOType ioType;
};

void LIBUSB_CALL asyncTransferCallback(struct libusb_transfer *transfer);

class AsyncIO : public AbstractIO
{
    Q_OBJECT
public:
    AsyncIO(libusb_device_handle *dev_handle,  unsigned int timeout, libusb_transfer_cb_fn cb = asyncTransferCallback, QObject *parent = nullptr);

    ~AsyncIO();

protected:
    libusb_device_handle *deviceHandle;
    int timeOut = 0;
    libusb_transfer *transfer = nullptr;
    libusb_transfer_cb_fn callback = nullptr;
    libusb_transfer_type type;
    QByteArray buffer;
    AsyncUserData userData;
};

class CtlIOAsync : public AsyncIO
{
    Q_OBJECT
public:
    CtlIOAsync(libusb_device_handle *dev_handle, unsigned int timeout = IO_TIME_OUT,  libusb_transfer_cb_fn cb = asyncTransferCallback, QObject *parent = nullptr);

    void doTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray &data = QByteArray());
};

class InterruptIOAsync : public AsyncIO
{
    Q_OBJECT
public:
    InterruptIOAsync(libusb_device_handle *dev_handle, unsigned int timeout = IO_TIME_OUT, libusb_transfer_cb_fn cb = asyncTransferCallback, QObject *parent = nullptr);

    void doTransfer(uint8_t endPoint, IOType ioType, const QByteArray& data = QByteArray());
};
 /*异步等时传输:
 * 不同的 USB 设备可能支持不同数量的等时数据包
 * USB 总线带宽：USB 的带宽是有限的，需要确保设置的 ISO_PACKET_COUNT 不会导致带宽超出设备或主机的处理能力。
 * USB 2.0 设备的带宽为 480 Mbps（60 MB/s），USB 3.0 的带宽为 5 Gbps（625 MB/s）。确保在设置数据包数量时考虑到这些带宽限制。
 * 数据包大小：在 USB 传输中，USB 规范规定了每个数据包的最大大小。对于 USB 2.0，最大数据包大小通常为 1024 字节（对于等时传输）
 * ，而对于 USB 3.0，最大数据包大小可以更大（最高可达 1024 字节或更多，根据端点类型）。
 * 确保你的 ISO_PACKET_COUNT 和每个数据包的大小相结合不会超出 USB 的最大传输限制。
 * 延迟和实时性要求：某些音频或视频处理应用可能需要更低的延迟，可能要使用较少的等时数据包以便更快地处理数据。
 * */
#define ISO_PACKET_COUNT 3      // libusb_get_max_iso_packet_size()  libusb_set_iso_packet_lengths()
#define ISO_SINGLE_PACKET_SIZE 3

class IsoIOAsync : public AsyncIO
{
    Q_OBJECT
public:
    IsoIOAsync(libusb_device_handle *dev_handle, unsigned int timeout = IO_TIME_OUT, libusb_transfer_cb_fn cb = asyncTransferCallback, QObject *parent = nullptr);

    void doTransfer(uint8_t endPoint, IOType ioType, uint8_t  pointNum, const QByteArray& data = QByteArray());
};