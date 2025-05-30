#include "encryptor.h"
#include "utils/logger/logger.h"
#include "utils/bytearray/bytearray.h"
#include <qcryptographichash.h>
#include <qdebug.h>

namespace Encryptor {
std::string encrypt(const std::string &data) {
    return encrypt("ant-tech", data);
}

std::string decrypt(const std::string &data) {
    return decrypt("ant-tech", data);
}

std::string encrypt(const std::string& key, const std::string &data) {
    QByteArray hashKey = QCryptographicHash::hash(QByteArray::fromStdString(key), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    QByteArray encrypted = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                                 QString::fromStdString(data).toUtf8(), hashKey, iv, QAESEncryption::PKCS7);
    auto res = encrypted.toStdString();
    qDebug() << "key=" << hashKey.toHex() << " iv=" << iv.toHex();
    LOG_INFO("Encryt: key=", ByteArray(hashKey.toStdString()).toHexStr(), "iv",
             ByteArray(iv.toStdString()).toHexStr(), "plain text=", data," cipher text=", ByteArray(res).toHexStr());
    return res;
}

std::string decrypt(const std::string& key, const std::string &data) {
    QByteArray hashKey = QCryptographicHash::hash(QString::fromStdString(key).toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    auto decData = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::CBC, QByteArray::fromStdString(data),
                                           hashKey, iv,  QAESEncryption::PKCS7);
    decData = QAESEncryption::RemovePadding(decData, QAESEncryption::PKCS7);
    auto res = decData.toStdString();
    LOG_INFO("Decode Res:", res);
    return res;
}

std::string encrypt(unsigned long sk, const std::string &data) {
    auto skStr = std::to_string(sk);
    return encrypt(skStr, data);
}

std::string decrypt(unsigned long sk, const std::string &data) {
    auto skStr = std::to_string(sk);
    return decrypt(skStr, data);
}

std::string sha256(const std::string& data) {
    auto hashVal = QCryptographicHash::hash(QByteArray::fromStdString(data), QCryptographicHash::Sha256).toStdString();
    LOG_DEBUG("SHA-256: plain text=", data, "cipher text=", hashVal, "cipher size:", hashVal.size());
    return hashVal;
}

std::string toBase64(const std::string& data) {
    return QByteArray::fromStdString(data).toBase64().toStdString();
}

std::string fromBase64(const std::string& data) {
    return QByteArray::fromBase64(QByteArray::fromStdString(data)).toStdString();
}

}
