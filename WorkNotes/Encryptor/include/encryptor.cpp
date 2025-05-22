#include "encryptor.h"
#include "src/utils/aes/encry_aes.h"
#include "src/utils/keymanager/keymanager.h"

namespace ET {
DECL_PREFIX auto& keyManager = KeyManager::instance();

DECL_PREFIX QByteArray encrypt(const QString& key, const QByteArray& data) {
    return AES::aesEncrypt(key, data);
}

DECL_PREFIX QByteArray decrypt(const QString& key, const QByteArray& data) {
    return AES::aesDecrypt(key, data);
}

//DECL_PREFIX void getSessionKey() {
//    keyManager.getKey();
//}
//
//DECL_PREFIX void getDeviceId() {
//    keyManager.getId();
//}
//
//DECL_PREFIX void refreshKey() {
//    keyManager.refreshKey();
//}

extern "C" {
DECL_PREFIX int getKey(char* result) {
    return keyManager.getKey(result);
}

DECL_PREFIX int getId(char* result) {
    return keyManager.getId(result);
}

DECL_PREFIX int refreshKeyUnix(const char* skAddr, char* result) {
    return keyManager.refreshKey(skAddr, result);
}

DECL_PREFIX int refreshKeyWin(char* result) {
    return keyManager.refreshKey(result);
}

}

}