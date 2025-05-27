#include "hostinfo.h"
#include <qfile.h>
#include <qprocess.h>
#include <iostream>
#include <qthread.h>
#include <qregularexpression.h>

namespace HostInfo {

std::string getCpuId() {
    return "12th Gen Intel(R) Core(TM) i5-12400";
}

std::string getMac() {
    return "EC:D6:8A:FE:56:D5";
}
}