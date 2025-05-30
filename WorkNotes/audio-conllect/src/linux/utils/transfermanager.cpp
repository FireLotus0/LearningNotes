#include "transfermanager.h"
#include "usbmanager.h"
#include "configadc.h"
#include <qdatetime.h>

//#define DEBUG_MODE

TransferManager &TransferManager::instance() {
    static TransferManager instance;
    return instance;
}

TransferManager::TransferManager(QObject *parent)
    : QObject(parent)
{
    CODEC.registerType<BytesCodec<HostData>>(this, &TransferManager::onRecvHostData);
    CODES.registerType<FrameData, BytesCodec>();
    connectSignals();
    fpgaData1Reader = new FpgaData1Reader(this);
    qDebug() << "BUFF_READ_FREQ=" << BUFF_READ_FREQ;
    sampleTimer.setInterval(BUFF_READ_FREQ);
    sampleTimer.callOnTimeout([&]{
        uploadData();
    });
    hostData.startSwitch = 0;

    // 连接usb信号
    connect(&UsbManager::instance(), &UsbManager::connectionChanged, this, [&](uint16_t pid, uint16_t vid, bool connected) {
        qInfo() << QString("USB(PID=%1, VID=%2)已%3").arg(pid).arg(vid).arg(connected ? "连接" : "断开");
    });

}

TransferManager::~TransferManager()
{
    stopServer();
}

void TransferManager::connectSignals() {
    connect(&server, &QTcpServer::newConnection, this, &TransferManager::onNewConnectionAccept);
    connect(&server, &QTcpServer::acceptError, this, [&](QAbstractSocket::SocketError error) {
        qDebug() << "accept error: " << server. errorString() << " " << error;
    });
}

void TransferManager::onNewConnectionAccept() {
    if(!clientSocket.isNull()) {
        return;
    }
    clientSocket = server.nextPendingConnection();
    if(!clientSocket) {
        qDebug() << "New client socket is null!";
        return;
    }
    connect(clientSocket, &QTcpSocket::readyRead, this, [&]() {
        auto data = clientSocket->readAll();
        qDebug() << "rawData:" << data;
        CODEC.appendBuffer(data);
    });
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::errorOccurred, clientSocket, [&](QAbstractSocket::SocketError error){
        qDebug() << "socket error: " << error << clientSocket->errorString();
    });
    qDebug() << "New client connected!";
//    QTimer::singleShot(2000, [&]{
//       qDebug() << "Sample Start!";
//       sampleTimer.start();
//    });
}

void TransferManager::onRecvHostData(const HostData &data) {
    // 上位机确保其他参数正确
    if(data.isControlSwitch == 1) { // 控制启停
        if(hostData.startSwitch != data.startSwitch) {
            if(data.startSwitch == 1) {
                sampleTimer.start();
                qDebug() << "start sample!";
                unsigned int start = 0x03;
                Xil_Out32(XPAR_BRAM_0_BASEADDR, 0x03);
//                fpgaData1Reader->writeToSharedMemory(XPAR_BRAM_0_BASEADDR, &start, 4); // 开始采样
            } else {
                sampleTimer.stop();
                qDebug() << "stop sample!";
                unsigned int stop = 0x00;
//                fpgaData1Reader->writeToSharedMemory(XPAR_BRAM_0_BASEADDR, &stop, 4); // 开始采样
                Xil_Out32(XPAR_BRAM_0_BASEADDR, 0x00);
            }
        }
    } else {
        uint32_t way = data.way;
        uint32_t points = data.points;
//        fpgaData1Reader->writeToSharedMemory(SAMPLE_WAY_ADDR, &way, 4); // 写入采集方式
        Xil_Out32(SAMPLE_WAY_ADDR, way);
//        fpgaData1Reader->writeToSharedMemory(SAMPLE_POINT_COUNT_ADDR, &points, 4); // 写入采样点数
        Xil_Out32(SAMPLE_POINT_COUNT_ADDR, points);
        //        100k  -  1869
        //        250k  -  669
        //        500k  -  269 （上电默认）
        //        1M    -  69
        //        1.5M  -  3   实际采样率为 1.49254MHz
        unsigned long rate = data.rate;
        switch(rate) {
            case 0: rate = 1869;   // 100k
            case 1: rate = 669; break;   // 250k
            case 2: rate = 269;break;    // 500k
            case 3: rate = 69;break;   // 1M
            case 4: rate = 3;break;   // 1.5M
            default:qWarning() << "Wrong Sample Rate!";
                Q_ASSERT(false);
        }
        auto r = (unsigned int)rate;
//        fpgaData1Reader->writeToSharedMemory(XPAR_BRAM_0_BASEADDR + 0x14, &r, 4); // 写入采集率
        Xil_Out32(XPAR_BRAM_0_BASEADDR + 0x14, rate);
    }
    hostData = data;
    qDebug() << "recv host data: " << data;
}

void TransferManager::startServer() {
    // 启动服务器并监听端口
    if (!server.listen(QHostAddress::Any, 12345)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }
    writeUsbTimer.setInterval(1000);
    writeUsbTimer.callOnTimeout([&]{
        auto dev = UsbManager::instance().getDevice(PID, VID);
        if(dev == nullptr) {
            qDebug() << "device ptr is null";
        } else  {
            QByteArray data(64, 6);
            dev->write(data);
        }
    });
    UsbManager::instance().appendInterestDev({{PID, VID}});
    QTimer::singleShot(500, [&] {
        qDebug() << "start find device!";
        UsbManager::instance().startSearch();
        writeUsbTimer.start();
    });
}

void TransferManager::stopServer() {
    if(clientSocket->isOpen()) {
        clientSocket->close();
    }
    if(server.isListening()) {
        server.close();
    }
    if(sampleTimer.isActive()) {
        sampleTimer.stop();
    }
    qDebug() << "Server Stopped!";
}

void TransferManager::uploadData() {
#ifdef DEBUG_MODE
    FrameData data;
    qDebug() << "upload one frame of data!" + QString::number(sizeof(data));
    if(isConnectionValid()) {
        clientSocket->write(CODES.encode<FrameData>(data));

    }
#else
//    if(isFirstSample) {
//        fpgaData1Reader->collectionBegin();
//        isFirstSample = false;
//    }
//    fpgaData1Reader->dataPositionUpdate();
//    auto data = fpgaData1Reader->packageData();

//    QByteArray d(2392721, '3');
//    if(isConnectionValid()) {
//        clientSocket->write(d);
//        clientSocket->flush();
//        clientSocket->waitForBytesWritten();
//    }
//    for(const auto& frame : data) {
//        if(isConnectionValid()) {
//            clientSocket->write(CODES.encode<FrameData>(frame));
////            qDebug() << "Frame size:" << CODES.encode<FrameData>(frame).size();
//        }
//    }
//    if(data.size() > 0) {
//        qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch() << "  send size:" << 2392721;
//    }

#endif
}
