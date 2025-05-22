#pragma once

#include <qstring.h>
#include <qstringlist.h>

#if defined(OS_WIN)
#include <windows.h>
#ifdef USE_LIB
#define DECL_PREFIX __declspec(dllimport)
#else
#define DECL_PREFIX __declspec(dllexport)
#endif
#elif defined(OS_LINUX)
#define DECL_PREFIX
#endif

enum class ErrCode {
    SUCCESS = 0,
    KEY_INVALID,
    SK_INVALID,
    ID_INVALID,
    DEVICE_INVALID,
    SERVER_INVALID,
    TOKEN_INVALID,
    DEVICE_LEFT,
    DEVICE_TIMEOUT,
    GEN_KEY_FAILED,
    USB_WRITE_ERROR,
    USB_READ_ERROR,
    READ_ID_FAILED,
    LOGIN_FAILED,
    PARSE_JSON_FAILED,
    NETWORK_REPLY_ERROR,
    NETWORK_REPLY_TIMEOUT,
    SERVER_ERROR,
};

enum CallBackType {
    READ_KEY,
    READ_ID,
    GENERATE_KEY
};

namespace ET {

// KeyManager* keyManager;

DECL_PREFIX QByteArray encrypt(const QString& key, const QByteArray& data);

DECL_PREFIX QByteArray decrypt(const QString& key, const QByteArray& data);

/// Windows平台非阻塞调用
//DECL_PREFIX void getSessionKey();
//
//DECL_PREFIX void getDeviceId();
//
//DECL_PREFIX void refreshKey();

extern "C" {
DECL_PREFIX int getKey(char* result);

DECL_PREFIX int getId(char* result);

DECL_PREFIX int refreshKeyUnix(const char* skAddr, char* result);

DECL_PREFIX int refreshKeyWin(char* result);

}
}

