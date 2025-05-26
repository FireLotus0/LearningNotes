#include "encryptor.h"
#include "utils/logger/logger.h"
#include <qcryptographichash.h>

namespace Encryptor {
std::string encrypt(const std::string &data) {
    return encrypt("ant-tech", data);
}

std::string decrypt(const std::string &data) {
    return decrypt("ant-tech", data);
}

std::string encrypt(const std::string& key, const std::string &data) {
    QByteArray hashKey = QCryptographicHash::hash(QString::fromStdString(key).toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    QByteArray encrypted = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                                 QString::fromStdString(data).toUtf8(), hashKey, iv, QAESEncryption::PKCS7);
    auto res = encrypted.toStdString();
    LOG_INFO("Encryt: key=", key, "plain text=", data," cipher text=", res);
    return res;
}

std::string decrypt(const std::string& key, const std::string &data) {
    QByteArray hashKey = QCryptographicHash::hash(QString::fromStdString(key).toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray iv{hashKey.constData(), 16};
    auto decData = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::CBC, QString::fromStdString(data).toUtf8(),
                                           hashKey, iv,  QAESEncryption::PKCS7);
    auto res = QAESEncryption::RemovePadding(decData, QAESEncryption::PKCS7).toStdString();
    LOG_INFO("Decrypt: key=", key, "cipher text=", data, "plain text=", res);
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
}
