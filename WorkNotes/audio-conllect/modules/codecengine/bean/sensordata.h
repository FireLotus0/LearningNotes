#pragma once

#include <qbytearray.h>
#include <qdatastream.h>
#include <qdebug.h>

struct AdcData {
    char adc[12];
    uint32_t turbine;

    AdcData() {
        char test = '1';
        memcpy(adc, &test, 12);
        turbine = 20;
    }
};

struct FrameData {
    enum {
        Type = 0
    };

    AdcData data[2048];

    QByteArray toBytes() const {
        return QByteArray((char*)data, 2048 * sizeof(AdcData));
    }

    static FrameData fromBytes(const QByteArray& data) {
        FrameData res{};
        memcpy(&res, data.data(), sizeof(data));
        return res;
    }
};

struct AdcData2 {
    uint32_t adc[4];
};