#include "encry_aes.h"
#include <qdebug.h>
#include <QRandomGenerator>


namespace AES {

#define SHARED_KEY QString("ant-tech")

QByteArray aesEncrypt(const QByteArray &data) {
    return aesEncrypt(SHARED_KEY, data);
}

QByteArray aesDecrypt(const QByteArray &encryptedData) {
    return aesDecrypt(SHARED_KEY, encryptedData);
}


QByteArray aesEncrypt(const QString& key, const QByteArray &data) {
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    QByteArray encrypted = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                                 data, hashKey, iv, QAESEncryption::PKCS7);
    return encrypted;
}

QByteArray aesDecrypt(const QString& key, const QByteArray &data) {
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    auto decData = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::CBC, data, hashKey, iv,  QAESEncryption::PKCS7);
    return QAESEncryption::RemovePadding(decData, QAESEncryption::PKCS7);
}

QByteArray aesEncrypt(quint64 sk, const QByteArray &data) {
    return aesEncrypt(QString::number(sk), data);
}

QByteArray aesDecrypt(quint64 sk, const QByteArray &data) {
    return aesDecrypt(QString::number(sk), data);
}

}