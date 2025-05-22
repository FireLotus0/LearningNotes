#pragma once

#include "src/utils/keymanager/keymanager.h"
#include "src/utils/logger/logger.h"
#include <qstring.h>
#include <qstringlist.h>

namespace ET {

// KeyManager* keyManager;

DECL_PREFIX QByteArray encrypt(const QString& key, const QByteArray& data);

DECL_PREFIX QByteArray decrypt(const QString& key, const QByteArray& data);

DECL_PREFIX void getSessionKey();

DECL_PREFIX void getDeviceId();

DECL_PREFIX void refreshKey();

extern "C" {
DECL_PREFIX int getKey(char* result);

DECL_PREFIX int refreshKey(const char* skAddr, char* result);
}
}

