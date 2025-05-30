#include "customcodeengine.h"

ControlCodeEngine::ControlCodeEngine(QObject *parent)
    : ProtocolCodecEngine(parent)
{
    frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
    setVerifyFlags("SC");
}
ControlCodeEngine &ControlCodeEngine::instance() {
    static ControlCodeEngine instance;
    return instance;
}


SampleCodeEngine::SampleCodeEngine(QObject *parent)
    : ProtocolCodecEngine(parent)
{
    frameDeclare("H(5AFF)S2CV(SUM16)E(FE)");
    setVerifyFlags("SC");
}
SampleCodeEngine &SampleCodeEngine::instance() {
    static SampleCodeEngine instance;
    return instance;
}