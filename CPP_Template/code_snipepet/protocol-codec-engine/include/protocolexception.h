#pragma once

#include "global.h"

#include <qexception.h>

PROTOCOL_CODEC_NAMESPACE_BEGIN

class ProtocolException : public QException {
public:
    explicit ProtocolException(QString reason): _reason(std::move(reason)) {}

    void raise() const override {
        throw* this;
    }

    QException* clone() const override {
        return new ProtocolException(*this);
    }

    QString reason() const {
        return _reason;
    }

private:
    QString _reason;
};

PROTOCOL_CODEC_NAMESPACE_END