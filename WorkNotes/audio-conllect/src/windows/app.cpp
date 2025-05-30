#include "app.h"

#include "../usb/usbmanager.h"
#include "bytetool/byteutil.h"
#include "utils/appsettings.h"

#include <qdatetime.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlist.h>

App::App(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    CODEC.registerType<HostData, BytesCodec>();
    CODES.registerType<BytesCodec<FrameData>>(this, &App::callbackFunc);
    ui.label_path->setText(AppSetting::General::lastSaveDir());
    monitorTcpComm();

    initUsb();
}

App::~App() {
    if (socket != nullptr) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            startGather(false, false);
        }

        socket->disconnectFromHost();
        socket->close();
        delete socket;
    }
}

void App::monitorTcpComm() {
    if (socket == nullptr) {
        socket = new QTcpSocket(this);
    }
    dlg = QSharedPointer<ConnectToServerDlg>(new ConnectToServerDlg(socket, this));
    connect(socket, &QTcpSocket::connected, [this]() {
        dlg->hide();
        setEnabled(true);
        receiveClientInfo();
    });
    connect(socket, &QTcpSocket::disconnected, [this]() {
        dlg->show();
        ui.btn_start->setText("开始采集");
    });
    connect(socket, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError socketError) {
        dlg->promptConnectFail(qt_error_string(socketError));
        if (!dlg.isNull()) {
            dlg->show();
        }
    });
    connect(dlg.get(), &ConnectToServerDlg::connectToServer, [this](bool isContinue) {
        if (!isContinue) {
            hide();
        }
    });
    dlg->show();
}

void App::receiveClientInfo() {
    if (socket != nullptr) {
        connect(socket, &QTcpSocket::readyRead, [&] {
            auto buff = socket->readAll();
//            CODES.appendBuffer(buff);
//            qDebug() << "receive data:" << buff.size();
        });
    }
}

void App::sendClientInfo(HostData p) {
    if (socket == nullptr) {
        return;
    }
    socket->write(CODEC.encode(p));
}

void App::startGather(bool isSet, bool ifStartGather) {
    HostData params{};
    auto sampleRateValues = ui.groupBox_1->findChildren<QRadioButton*>();
    auto wayValues = ui.groupBox_4->findChildren<QRadioButton*>();

    for (auto sampleRateValue : sampleRateValues) {
        if (sampleRateValue->isChecked()) {
            auto str = sampleRateValue->objectName();
            params.rate = str.rightRef(1).toInt() - 1;
            break;
        }
    }
    for (auto wayValue : wayValues) {
        if (wayValue->isChecked()) {
            auto str = wayValue->objectName();
            params.way = str.rightRef(1).toInt() - 1;
            break;
        }
    }
    params.points = ui.spinBox->value();
    params.startSwitch = ifStartGather ? 1 : 0;
    params.isControlSwitch = !isSet ? 1 : 0;
    sendClientInfo(params);
}

void App::saveDataToBin(const QByteArray &data, int channel) {
    if(saveFile == nullptr) {
        return;
    }
    if (saveFile->size() == 0) {
        QJsonArray arr;
        for (int i = 0; i < channel; i++) {
            arr.append(QJsonValue("int32_t"));
        }
        arr.append(QJsonValue("uint32_t"));
        QJsonObject obj;
        obj.insert("formatType", 5);
        obj.insert("dataSize", channel * 4 + 4);//当前数据大小，应为int_32的字节数加上uint_32的字节数
        obj.insert("types", arr);
        auto headerInfo = QJsonDocument(obj).toJson();
        headerInfo.append(256 - headerInfo.size(), '\0');
        saveFile->write(headerInfo);
    }
    saveFile->write(data);
}

void App::on_btn_set_clicked() {
    startGather(true);
}

void App::on_btn_path_clicked() {
    QString folderPath = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"), QDir::homePath());
    if (!folderPath.isEmpty()) {
        AppSetting::General::lastSaveDir = folderPath;
    }
    ui.label_path->setText(folderPath);
}

void App::on_btn_start_clicked() {
    if (ui.radioButton_channel_1->isChecked() || ui.radioButton_channel_2->isChecked() || ui.radioButton_channel_3->isChecked() || ui.radioButton_channel_4->isChecked()) {
        if (ui.btn_start->text().contains("开始")) {
            if (saveFile) {
                saveFile->close();
                delete saveFile;
                saveFile = nullptr;
            }
            QDir dir(AppSetting::General::lastSaveDir());
            if (!dir.exists()) {
                dir.mkpath(".");
            }
            auto fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".bin";
            saveFile = new QFile(dir.path() + "/" + fileName);
            if (!saveFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QMessageBox::warning(this, "提示", QString("文件打开失败: 文件名[%1]  目录[%2] Error[%3]").arg(fileName).arg(dir.path()).arg(saveFile->errorString()));
                delete saveFile;
                saveFile = nullptr;
                return;
            }
            startGather(false, true);
            ui.btn_start->setText("结束采集");
        } else {
            if (saveFile) {
                saveFile->close();
                delete saveFile;
                saveFile = nullptr;
            }
            startGather(false, false);
            ui.btn_start->setText("开始采集");
        }
    } else {
        QMessageBox::warning(this, "提示", "请选择采集通道");
    }
}

void App::callbackFunc(const FrameData& data) {
    QList<int> list{};
    if (ui.radioButton_channel_1->isChecked()) {
        list.append(0);
    }
    if (ui.radioButton_channel_2->isChecked()) {
        list.append(1);
    }
    if (ui.radioButton_channel_3->isChecked()) {
        list.append(2);
    }
    if (ui.radioButton_channel_4->isChecked()) {
        list.append(3);
    }
    QByteArray bytes;
    for (auto &it : data.data) {
        int32_t adc[4];
        for (int i = 0; i < 4; i++) {
            memcpy(&adc[i], it.adc + i * 3, 3);
            ByteUtil::swapBits(adc[i]);
        }
        for (auto channel : list) {
            bytes.append(reinterpret_cast<char*>(&adc[channel]), 4);
        }
        ByteUtil::swapBits(it.turbine);
        bytes.append((char*)(&it.turbine), 4);
    }
    saveDataToBin(bytes, list.size());
}

void App::initUsb() {
    UsbManager::instance().appendInterestDev({{PID, VID}});
    readUsb.setInterval(1000);
    readUsb.callOnTimeout([&]{
       auto dev = UsbManager::instance().getDevice(PID, VID);
       if(dev == nullptr) {
           qDebug() << "device not exist!";
       } else {
           dev->read(1);
       }
    });
    connect(&UsbManager::instance(), &UsbManager::connectionChanged, this, [&](uint16_t pid, uint16_t vid, bool connected) {
       QMessageBox::information(this, "提示", QString("USB(PID=%1, VID=%2)已%3").arg(pid).arg(vid).arg(connected ? "连接" : "断开"));
       if(connected) {
           readUsb.start();
       } else {
           readUsb.stop();
       }
    });
    connect(&UsbManager::instance(), &UsbManager::readFinished, this, [&](const QByteArray& data) {
        qDebug() << "read usb finish: " << data;
    });
    QTimer::singleShot(500, [&] { UsbManager::instance().startSearch(); });
}
