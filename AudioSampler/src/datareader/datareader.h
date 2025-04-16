#pragma once

#include "src/datacache/datacache.h"
#include "src/bean/datadef.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class DataReader : public Utils::SingleInstance<DataReader>
{
public:
    explicit DataReader(std::string&& mapFile);

    ~DataReader() override;
private:
    DataCache<SampleData>* dataCachePtr;
    std::string mapFilePath;
    int fileFd = -1;
    std::size_t pageSize;
};