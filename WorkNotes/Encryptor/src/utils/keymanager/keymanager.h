#pragma once

#include "src/utils/usbmanager/usbmanager.h"
#include "encryptor.h"
#include <qobject.h>
#include <qrandom.h>
#include <qstring.h>
#include <qmap.h>
#include <qmetaobject.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>

namespace ET {

#define p 1371857489UL
#define g 451517198UL

class DHCodec {
public:
    static quint64 createPrivateKey() {
        return g + QRandomGenerator64::global()->generate() % (p - g + 1);
    }

    static quint64 getPublicKey(quint64 privateKey) {
        return modPow(g, privateKey, p);
    }

    static quint64 getSharedKey(quint64 publicKey, quint64 privateKey) {
        return modPow(publicKey, privateKey, p);
    }

private:
    static quint64 modPow(quint64 x, quint64 y, quint64 n) {
        quint64 result = 1;
        x %= n;

        while (y > 0) {
            if (y & 1) {
                result = (result * x) % n;
            }
            x = (x * x) % n;
            y >>= 1;
        }
        return result;
    }
};

class DECL_PREFIX KeyManager : public QObject {
Q_OBJECT
    enum ServerStage {
        NONE,
        SEND_PUB_KEY,
        SEND_SESSION_KEY,
        LOGIN
    };

private:
    explicit KeyManager(QObject *parent = nullptr);

private:
    quint64 sk{0};
    bool isUserAuthed{false};
    QString key, id, token;
    QPair<uint16_t, uint16_t> devId;
    quint64 privateKey, publicKey, serverPubKey;
    QNetworkAccessManager networkManager;
    QString userName, password;

public:
    static KeyManager &instance();

/// *********************Windows与Unix共用阻塞调用*********************
public:
    /*!
    * @note 同步读取加密狗秘钥
    * @param result
    * @return
    */
    int getKey(char *result);

    /*!
     * @note 同步读取加密狗ID
     * @param result
     * @return
     */
    int getId(char *result);


/// *********************Windows平台阻塞调用*********************
public:
    int encrypt(char *res, const char *content);

    int decrypt(char *res, const char *content);

    int getToken(char *res);

    /*!
    * @note Windows平台同步刷新秘钥
    * @param skAddr
    * @param result 新秘钥
    * @return
    */
    int refreshKey(char *result);

    int login(const char* user, const char* passwd);
private:
    int sendPubKeyToServerSync();
    int onReplyFinishedSync(QNetworkReply *reply);
    int generateKeySync();
    int sendSessionKeyToSeverSync();
    int loginSync();
/// *********************Unix平台阻塞调用*********************
public:
    /*!
     * @note Linux平台同步刷新秘钥
     * @param skAddr
     * @param result 新秘钥
     * @return
     */
    int refreshKey(const char *skAddr, char *result);

/// *********************windows平台非阻塞调用*********************
signals:
    void errorOccurred(const QString &err, int errCode);

    void loginFinished(bool success, int errCode);

    void readKeyFinished(const QString &key, int errCode);

    void refreshKeyFinished(const QString &key, int errCode);

    void readIdFinished(const QString &key, int errCode);

    void deviceRemoved();

    void deviceAttached();
private:
    void getId();
    void getKey();
    void refreshKey();
    void login(const QString &user, const QString &passwd);
    ErrCode encrypt(QByteArray &res, const QByteArray &content);
    ErrCode decrypt(QByteArray &res, const QByteArray &content);
    QString getToken();

private:
    void sendPubKeyToServer();
    void sendSessionKeyToSever();
    void onReplyFinished(QNetworkReply *reply);
    void generateKey();
    void login();
};
}