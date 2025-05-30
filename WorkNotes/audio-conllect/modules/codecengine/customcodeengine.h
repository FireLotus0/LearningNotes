#pragma once

#include "bean/hostdata.h"
#include "bean/sensordata.h"
#include "protocolcodecengine.h"
#include <qobject.h>

PROTOCOL_CODEC_USING_NAMESPACE

#define CODEC ControlCodeEngine::instance()
#define CODES SampleCodeEngine::instance()

class ControlCodeEngine : public ProtocolCodecEngine
{
public:
    static ControlCodeEngine& instance();

private:
    explicit ControlCodeEngine(QObject *parent = nullptr);
};

class SampleCodeEngine : public ProtocolCodecEngine
{
public:
    static SampleCodeEngine& instance();

private:
    explicit SampleCodeEngine(QObject *parent = nullptr);
};

