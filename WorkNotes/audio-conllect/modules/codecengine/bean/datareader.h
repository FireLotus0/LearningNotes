#pragma once

#include "fpgacontroldata.h"
#include <qdebug.h>
#include <qobject.h>

#define BUFF_READ_FREQ  100 //ms
#define SAMPLE_WAY_ADDR 0x82000008  // 采集方式
#define SAMPLE_POINT_COUNT_ADDR 0x8200000c   // 采样点数

class DataReader : public QObject {
public:
    explicit DataReader(QObject *parent = nullptr);
    ~DataReader();

    void collectionBegin();

    void dataPositionUpdate();

    void enableSizeDebug() {
        printSizeDebug = true;
    }

    static void setFpga(const FpgaControlData& data);
    static FpgaControlResult readControlResult();
    static void copyFromSharedMemory(size_t address, void* dataPtr, size_t dataSize);
    static void writeToSharedMemory(size_t address, void* dataPtr, size_t dataSize);
protected:
    virtual size_t memoryBeginAddress() = 0;
    virtual size_t offsetStoredAddress() = 0;

    virtual size_t cycleBufferQueueSize() = 0;

    virtual QString who() = 0;

    void initReadBuf(size_t typeSize);

    template<typename T>
    QList<T> packageLastStoredData() {
        auto ptrSize = sizeof(T);
        size_t readSize;
        packageLastStoredData(ptrSize, readSize);
        if (readSize == 0) {
            return {};
        }

        QList<T> readData;
        for (int i = 0; i < readSize; i++) {
            readData << reinterpret_cast<T*>(readBuff)[i];
        }
        return readData;
    }

    void packageLastStoredData(size_t typeSize, size_t& readSize);

    template<typename T>
    static QByteArray dataHexStr(T data) {
        QByteArray d((char*)&data, sizeof(T));
        for (int i = 0; i < d.size() / 2; i++) {
            auto tmp = d.at(i);
            d[i] = d[d.size() - 1 - i];
            d[d.size() - 1 - i] = tmp;
        }
        return "0x" + d.toHex().toUpper();
    }

private:
    size_t lastReadPos;
    size_t oldReadPos;
    int8_t* readBuff;

    bool printSizeDebug = false;
};
