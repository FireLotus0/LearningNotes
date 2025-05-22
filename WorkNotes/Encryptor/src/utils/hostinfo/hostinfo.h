#pragma once

#include <qcoreapplication.h>
#include <qnetworkinterface.h>
#include <qtextstream.h>

#if defined(OS_WIN)
#include <windows.h>
#include <qoperatingsystemversion.h>
#endif

namespace ET {
namespace HostInfo {

QString getCpuId();

QString getMac();
}
}
