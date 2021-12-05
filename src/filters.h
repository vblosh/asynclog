#pragma once
#include "interfaces.h"
#include <unordered_map>
#include <string>

namespace simplelogger
{

class AreaFilter : public IFilter
{
    std::unordered_map<std::string, LogLevel> areaFilter;

public:
    bool Enabled(const Logdata& data);
    void SetFilter(const std::string& area, LogLevel level);
};

}