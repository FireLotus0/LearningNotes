#include <protocolcodecengine.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include "encry_aes.h"
#include <qnetworkreply.h>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include "encryptor.h"

PROTOCOL_CODEC_USING_NAMESPACE

class HttpClient : public QObject {
Q_OBJECT

public:
    HttpClient() {
        // 创建 QNetworkAccessManager 实例
        manager = new QNetworkAccessManager(this);

        // 发起 GET 请求
        QNetworkRequest request(QUrl("https://www.baidu.com"));
        QNetworkReply *reply = manager->get(request);

        // 连接 reply 的 finished 信号到处理函数
        connect(reply, &QNetworkReply::finished, this, &HttpClient::onFinished);
    }

private slots:
    void onFinished() {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        if (reply) {
            // 检查请求是否成功
            if (reply->error() == QNetworkReply::NoError) {
                // 获取响应数据
                QByteArray responseData = reply->readAll();
                qDebug() << "Response data:" << responseData;
            } else {
                qDebug() << "Error:" << reply->errorString();
            }
            reply->deleteLater(); // 清理 reply 对象
        }
    }

private:
    QNetworkAccessManager *manager;
};

struct GenKey {
    enum {
        Type = 2
    };

    QString cpuId;
    QString mac;
    QString sk;

    //json数据解码
    static GenKey fromJson(const QJsonDocument& doc) {
        GenKey tmp;
        tmp.cpuId = doc.object().value("cpuId").toString();
        tmp.mac = doc.object().value("mac").toString();
        tmp.sk = doc.object().value("sk").toString();
        return tmp;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("cpuId", cpuId);
        obj.insert("mac", mac);
        obj.insert("sk", sk);
        return QJsonDocument(obj);
    }
};

template<typename T>
class EncryptCodec : public protocol_codec::ProtocolTypeCodec<T> {
public:
    T decode(const QByteArray &content) override {
        auto tmp = AES::aesDecrypt(content);
        auto doc = QJsonDocument::fromJson(QString(tmp).toUtf8());
//        LOG_INFO("raw data:", QString(QString(content).toUtf8()), "decode:", QString(QString(tmp).toUtf8()));
        return T::fromJson(doc);
    }

    QByteArray encode(const T &data) override {
        return AES::aesEncrypt(data.toJson().toJson());
    }
};

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);
    protocol_codec::ProtocolCodecEngine tmpCodecEngine;
    tmpCodecEngine.frameDeclare("H(AAFF)S1CV(CRC16)E(AF)");
    tmpCodecEngine.setVerifyFlags("HSC");

    tmpCodecEngine.registerType<GenKey, EncryptCodec>();
    GenKey genKey;
    genKey.cpuId = "12th Gen Intel(R) Core(TM) i5-12400";
    genKey.mac = "EC:D6:8A:FE:56:D5";
    genKey.sk = "765478";

    char data[129];
    qDebug() << "id is " << ET::getId(data) << QString::fromStdString(std::string(data));

    HttpClient client; // 创建 HttpClient 实例
    qDebug() << "Code Data:" << tmpCodecEngine.encode<GenKey>(genKey).toHex();
    return a.exec();
}

#include "main.moc"