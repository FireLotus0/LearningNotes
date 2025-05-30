#pragma once

#include "threadmanage.h"
#include "codecengine/customcodeengine.h"
#include "codecengine/bean/fpgadatareader.h"

#include <qtcpsocket.h>
#include <qpointer.h>
#include <qtimer.h>
#include <qtcpserver.h>

#define PHYSICAL_BASE_ADDR 0x1000
#define PAGE_SIZE 4096
#define PACKAGE_SIZE 1024
#define PACKAGE_COUNT 1000

class TransferManager : public QObject
{
    Q_OBJECT
public:
    static TransferManager& instance();

    void startServer();

    void stopServer();

private:
    explicit TransferManager(QObject *parent = nullptr);

    ~TransferManager();

    void connectSignals();

    void onRecvHostData(const HostData& data);

    void uploadData();

    bool isConnectionValid() const {
        return !clientSocket.isNull() && clientSocket->error() == QAbstractSocket::SocketError::UnknownSocketError;
    }

private slots:
    void onNewConnectionAccept();

private:
    HostData hostData;
    QTcpServer server;
    QPointer<QTcpSocket> clientSocket;
    QTimer sampleTimer, writeUsbTimer;
    FpgaData1Reader* fpgaData1Reader;
    bool isFirstSample = true;
};
