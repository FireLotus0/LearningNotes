#pragma once

#include "src/qtaes/qaesencryption.h"
#include <qcryptographichash.h>
#include <qobject.h>
#include "encryptor.h"

namespace AES {

DECL_PREFIX QByteArray aesEncrypt(const QByteArray &data);

DECL_PREFIX QByteArray aesDecrypt(const QByteArray &data);

DECL_PREFIX QByteArray aesEncrypt(const QString& key, const QByteArray &data);

DECL_PREFIX QByteArray aesDecrypt(const QString& key, const QByteArray &data);

DECL_PREFIX QByteArray aesEncrypt(quint64 sk, const QByteArray &data);

DECL_PREFIX QByteArray aesDecrypt(quint64 sk, const QByteArray &data);
}