#include "uploaddatathr.h"
#include "arrayobjectcache.h"
#include <qpointer.h>

UploadDataThr::UploadDataThr(ArrayObjectCache<FrameData, 1000> *dataCachePtr, QMutex *mt, std::atomic_bool *startFlag, QTcpSocket **socket, QObject *parent)
    : dataCachePtr(dataCachePtr), mt(mt), startFlag(startFlag), socket(socket), QThread(parent) {
}

void UploadDataThr::run() {
    while (!isInterruptionRequested()) {
        if(startFlag->load(std::memory_order_acquire)) {
            QMutexLocker locker(mt);
            if (socket == nullptr) {
                qDebug() << "socket is null in sub thread";
                locker.unlock();
            } else {
                for (const auto &data: dataCachePtr->getCached()) {
                    if (((*socket)->write(CODES.encode<FrameData>(data))) != -1) {
                        (*socket)->waitForBytesWritten();
                    } else {
                        qCritical() << "Write Error:" << (*socket)->errorString();
                        break;
                    }
                }
            }
        }
    }
}
