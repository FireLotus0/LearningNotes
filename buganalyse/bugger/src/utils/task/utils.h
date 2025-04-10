#pragma once

#include <string>
#include <vector>

namespace Utils {

std::string curTime();

std::vector<char> readFile(const std::string& filePath);

}