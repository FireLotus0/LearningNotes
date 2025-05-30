#pragma once

#include "threadmanage.h"
#include "codecengine/customcodeengine.h"
#include <qtcpsocket.h>
#include <qobject.h>
#include <atomic>

template<typename T, int allocSize>
class ArrayObjectCache;

class UploadDataThr : public QThread
{
public:
    UploadDataThr(ArrayObjectCache<FrameData, 1000> *dataCachePtr, QMutex* mt, std::atomic_bool* startFlag, QTcpSocket** socket = nullptr, QObject* parent = nullptr);

protected:
    void run() override;
private:
    ArrayObjectCache<FrameData, 1000> *dataCachePtr;
    QMutex* mt;
    QTcpSocket** socket;
    std::atomic_bool * startFlag;
};