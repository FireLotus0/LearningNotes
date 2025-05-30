#pragma once

#include <qglobal.h>

struct FpgaControlData {
    uint8_t enableCollection: 1;
    uint8_t geoCalibrate: 1;
    uint8_t collectionMode: 1;
    uint8_t rtcControl: 1;

    static FpgaControlData beginCollection(uint8_t collectionMode) {
        FpgaControlData d = {0};
        d.enableCollection = 1;
        d.collectionMode = collectionMode;
        return d;
    }

    static FpgaControlData stopCollection() {
        FpgaControlData d = {0};
        return d;
    }

    static FpgaControlData beginGeoCalibrate() {
        FpgaControlData d = {0};
        d.geoCalibrate = 1;
        return d;
    }

    static FpgaControlData beginRtcCalibrate() {
        FpgaControlData d = {0};
        d.rtcControl = 1;
        return d;
    }
};

struct FpgaControlResult {
    uint8_t geo1: 1;
    uint8_t geo2: 1;
    uint8_t rtc: 1;
};