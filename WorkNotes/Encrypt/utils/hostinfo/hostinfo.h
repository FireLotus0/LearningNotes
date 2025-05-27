#pragma once

#include <qcoreapplication.h>
#include <qnetworkinterface.h>
#include <qtextstream.h>

#if defined(OS_WINDOWS)
#include <windows.h>
#include <qoperatingsystemversion.h>
#endif

namespace HostInfo {

std::string getCpuId();

std::string getMac();
}
