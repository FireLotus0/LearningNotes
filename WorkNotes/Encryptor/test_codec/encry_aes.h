#pragma once

#include "qtaes/qaesencryption.h"
#include <qcryptographichash.h>
#include <qobject.h>

namespace AES {

QByteArray aesEncrypt(const QByteArray &data);

QByteArray aesDecrypt(const QByteArray &data);

QByteArray aesEncrypt(const QString& key, const QByteArray &data);

QByteArray aesDecrypt(const QString& key, const QByteArray &data);

QByteArray aesEncrypt(quint64 sk, const QByteArray &data);

QByteArray aesDecrypt(quint64 sk, const QByteArray &data);
}