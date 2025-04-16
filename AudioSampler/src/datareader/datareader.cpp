
#include "datareader.h"
#include "src/logger/logger.h"

DataReader::DataReader(std::string &&mapFile)
    : mapFilePath(std::move(mapFile))
{
    dataCachePtr = DataCache<SampleData>::instance();
    pageSize = sysconf(_SC_PAGESIZE);
    INFO("Sys Page Size:", pageSize);
}

DataReader::~DataReader() {
    if(fileFd != -1) {
        close(fileFd);
    }
}
