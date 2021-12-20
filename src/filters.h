#pragma once
#include "interfaces.h"
#include <unordered_map>
#include <string>

namespace asynclog
{

class AreaFilter : public IFilter
{
    std::unordered_map<std::string, LogLevel> areaFilter;
    LogLevel logLevel;

public:
    AreaFilter() : logLevel(LogLevel::TRACE) {}
    bool Enabled(const Logdata& data) override;
    void SetReportingLevel(LogLevel level) override;
    void SetFilter(const std::string& area, LogLevel level);
};

}