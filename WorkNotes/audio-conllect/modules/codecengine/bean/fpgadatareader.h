#pragma once

#include "datareader.h"
#include "../bean/sensordata.h"

#include <qobject.h>

class FpgaData1Reader : public DataReader {
public:
    explicit FpgaData1Reader(QObject* parent = nullptr);

    QList<FrameData> packageData();

protected:
    size_t memoryBeginAddress() override;

    size_t offsetStoredAddress() override;

    size_t cycleBufferQueueSize() override;

    QString who() override;
};
