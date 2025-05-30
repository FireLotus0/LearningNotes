#pragma once

#include <qcoreapplication.h>
#include <qnetworkinterface.h>
#include <qtextstream.h>

namespace HostInfo {

std::string getCpuId();

std::string getMac();
}
