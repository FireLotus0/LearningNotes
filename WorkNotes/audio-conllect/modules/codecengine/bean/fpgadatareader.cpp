#include "fpgadatareader.h"

FpgaData1Reader::FpgaData1Reader(QObject *parent)
    : DataReader(parent)
{
    initReadBuf(sizeof(FrameData));
}

size_t FpgaData1Reader::memoryBeginAddress() {
    return 0x60000000;
}

size_t FpgaData1Reader::offsetStoredAddress() {
    return 0x82000100;
}

size_t FpgaData1Reader::cycleBufferQueueSize() {
    return 100 * 2 * 5; //1ms存5个，两倍缓存
}

QList<FrameData> FpgaData1Reader::packageData() {
    return packageLastStoredData<FrameData>();
}

QString FpgaData1Reader::who() {
    return "Fpga1";
}
