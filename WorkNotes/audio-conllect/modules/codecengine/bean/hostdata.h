#pragma once

#include <qbytearray.h>
#include <qdatastream.h>
#include <qdebug.h>

// PC端下发数据
struct HostData{
    enum {
        Type = 0
    };
    uint32_t rate:3;
    uint32_t range:2;
    uint32_t channel:2;
    uint32_t way:1;
    uint32_t startSwitch:1; // 开始采集标记
    uint32_t isControlSwitch:1;   // 区分控制开始采集和参数设置
    uint32_t points;

    QByteArray toBytes() const {
        int sz = sizeof(HostData);
        QByteArray res(sz, 0);
        memcpy(res.data(), this, sz);
        return res;
    }

    static HostData fromBytes(const QByteArray& data) {
        HostData res{};
        memcpy(&res, data.data(), sizeof(HostData));
        return res;
    }

    friend QDebug operator<<(QDebug qDebug1, const HostData& data) {
        qDebug1 << "Host Data[ rate=" <<data.rate << ", range=" << data.range << ", channel=" <<data.channel << ", way=" << data.way << ", points=" << data.points
                << ", startSwitch=" << data.startSwitch << ", isControlSwitch:" << data.isControlSwitch;
        return qDebug1;
    }
};