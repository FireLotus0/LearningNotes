#include "datareader.h"


#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <qthread.h>
#include <qdatetime.h>
#include <qdebug.h>

DataReader::DataReader(QObject *parent)
    : QObject(parent)
    , lastReadPos(0)
    , oldReadPos(0)
    , readBuff(nullptr)
{
                      enableSizeDebug();
}

DataReader::~DataReader() {
    qDebug() << "free readBuff";
    delete readBuff;
    qDebug() << "free readBuff finished";
}

/**
 * 开始采集时读取一次当前偏移记录值，从该位置开始打包
 */
void DataReader::collectionBegin() {
    copyFromSharedMemory(offsetStoredAddress(), &lastReadPos, 4);
    qInfo() << "Collection Begin: last read pos:" << lastReadPos;
}

/**
 * 更新数据偏移量
 */
void DataReader::dataPositionUpdate() {
    if(oldReadPos == lastReadPos) {
        qDebug() << "pos no update!";
    }
    oldReadPos = lastReadPos;
//    qDebug() << "start update pos";
    copyFromSharedMemory(offsetStoredAddress(), &lastReadPos, 4);
//    qDebug() << "update pos finish";
}

#define CONTROL_ADDR  0xA0000000
/**
 * 控制fpga
 * @param data 控制命令数据
 */
void DataReader::setFpga(const FpgaControlData &data) {
    writeToSharedMemory(CONTROL_ADDR, (char*)&data, sizeof(FpgaControlData));
}

/**
// * 读取fpga校准控制结果
// * @return 校准控制结果数据
// */
//FpgaControlResult DataReader::readControlResult() {
//    FpgaControlResult result = {0};
//    copyFromSharedMemory(CONTROL_RST_ADDR, &result, sizeof(FpgaControlResult));
//    return result;
//}


///**
// * 从fpga中读取当前时间
// * @return
// */
//qint64 DataReader::readTimeFromFpga() {
//    FpgaDateTime dateTime = {0};
//    copyFromSharedMemory(FPGA_CUR_DATETIME_ADDR, &dateTime, sizeof(FpgaDateTime));
//    qInfo() << "read datetime from fpga:" << dateTime.toString();
//    return dateTime.toTimestamp();
//}
//
///**
// * 将时间戳写入fpga
// * @param timestamp FpgaDateTime结构体数据
// */
//void DataReader::setTimeToFpga(qint64 timestamp) {
//    FpgaDateTime dateTime = {0};
//    //dateTime.fromTimestamp(timestamp);
//    memcpy(&dateTime, &timestamp, sizeof(FpgaDateTime));
//    qInfo() << "set datetime to fpga:" << dateTime.toString();
//    writeToSharedMemory(FPGA_CALIBRATE_DATETIME_ADDR, &dateTime, sizeof(FpgaDateTime));
//}

/**
 * 初始化读取缓存区大小，用于存储最后一次打包的所有数据
 * @param typeSize 通常是打包结构体的数据大小，sizeof(FpgaData1)或sizeof(FpgaData2)
 */
void DataReader::initReadBuf(size_t typeSize) {
    qDebug() << "malloc size：" << cycleBufferQueueSize() * typeSize;
    readBuff = (int8_t*) malloc(cycleBufferQueueSize() * typeSize);
}

/**
 * 读取上次位置到当前位置之间的所有数据，存储到readBuff缓存中，返回读取大小
 * @param typeSize 每个数据包结构体大小
 * @param readSize 总共读取的结构体包数量
 */
void DataReader::packageLastStoredData(size_t typeSize, size_t& readSize) {

    //检查是否有数据可读，无脉冲触发没有数据
    if (oldReadPos == lastReadPos) {
        readSize = 0;
        if (printSizeDebug) {
            qWarning() << who() << "has no data to pkg!";
        }
        return;
    }

    //上一次缓存pos
    auto left = (oldReadPos - memoryBeginAddress()) / typeSize;
    //当前缓存pos
    auto right = (lastReadPos - memoryBeginAddress()) / typeSize;

    readSize = right >= left ? (right - left) : (cycleBufferQueueSize() - left + right);
    if (printSizeDebug) {
        qDebug() << "pkg size:" << readSize << "left:" << left << "right:" << right << " cycleBufferQueueSize:" << cycleBufferQueueSize() << " typeSize:" << typeSize;
    }

    if (left < right) {
        //上次位置在左侧，当前位置在右侧，中间的数据即为上次缓存的所有数据
        auto memorySize = readSize * typeSize;
        copyFromSharedMemory(memoryBeginAddress() + left * typeSize, readBuff, memorySize);
    } else {
        //上次位置在右侧，当前位置在左侧，则分段读取数据
        auto leftMemorySize = cycleBufferQueueSize() - left;
        copyFromSharedMemory(memoryBeginAddress() + left * typeSize, readBuff, leftMemorySize * typeSize);
        auto rightMemorySize = readSize - leftMemorySize;
        if (rightMemorySize != 0) {
            copyFromSharedMemory(memoryBeginAddress(), readBuff + leftMemorySize * typeSize, rightMemorySize * typeSize);
        }
    }
}

#define MAP_MASK    0xfff  //align address to 4096

/**
 * 从指定位置读取共享内存数据
 * @param address 共享内存映射起始位置
 * @param dataPtr 数据缓存内存地址
 * @param dataSize 读取共享内存字节总数
 */
void DataReader::copyFromSharedMemory(size_t address, void *dataPtr, size_t dataSize) {
//    qDebug() << "start copyFromSharedMemory:" << address << " - " << dataSize;
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        qFatal("open shared memory failed!");
    }

    //计算偏移量，多读取offset个使得读取地址是片大小（4096）的整数倍
    auto addressOffset = address & MAP_MASK;
    //将映射长度对齐片大小
    auto mapSize = ((dataSize + addressOffset) & ~MAP_MASK) + (MAP_MASK + 1);

    void* mapData = mmap(nullptr, mapSize, PROT_READ, MAP_SHARED | MAP_LOCKED, fd, (long)address & ~MAP_MASK);
    if (mapData == (void*)-1) {
        qCritical() << "read mmap from" << dataHexStr(address) << "failed! size:" << dataSize << " mpSize:" << mapSize;
        printf("mmap error: %d - %s\n", errno, strerror(errno));
        qFatal("error! map shared memory failed!");
    }

    char* virtualAddr = (char*)mapData + addressOffset;
    /*if (address == offsetStoredAddress() && dataSize == 4) {
        qDebug() << "prepare copy data! from:" << dataHexStr(address)
                 << "size:" << dataSize
                 << "map size:" << mapSize
                 << "offset:" << dataHexStr(addressOffset);
        QByteArray debugData((char*)mapData, mapSize);
        qDebug() << debugData.toHex(' ');
    }*/

    memcpy(dataPtr, virtualAddr, dataSize);

    if (munmap(mapData, mapSize) == -1) {
        printf("mmap error: %d - %s\n", errno, strerror(errno));
        qFatal("error! unmap shared memory failed!");
    }
    close(fd);
//    qDebug() << "finish copyFromSharedMemory!";
}

/**
 * 将数据写入到共享内存指定位置
 * @param address 共享内存映射起始位置
 * @param dataPtr 待写入内存地址
 * @param dataSize 写入字节数
 */
void DataReader::writeToSharedMemory(size_t address, void *dataPtr, size_t dataSize) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        qFatal("open shared memory failed!");
    }

    //计算偏移量，多读取offset个使得读取地址是片大小（4096）的整数倍
    auto addressOffset = address & MAP_MASK;

    void* mapData = mmap(nullptr, dataSize + addressOffset, PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, address & ~MAP_MASK);
    if (mapData == (void*)-1) {
        qCritical() << "mmap from" << dataHexStr(address) << "failed! size:" << dataSize;
        printf("mmap error: %d - %s\n", errno, strerror(errno));
        qFatal("error! map shared memory failed!");
    }

    char* virtualAddr = (char*)mapData + addressOffset;
    for (int i = 0; i < dataSize; i++) {
        *(virtualAddr + i) = *((char *) dataPtr + i);
        qWarning() << "address write:" << dataHexStr(address + i) << "data:"
                   << dataHexStr(*((char *) dataPtr + i));
    }

    munmap(mapData, dataSize + addressOffset);
    close(fd);
}
