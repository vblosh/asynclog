#pragma once
#include "interfaces.h"
#include <vector>
#include <string>

namespace asynclog
{

class AreaFilter : public IFilter
{
    std::vector<LogLevel> areaFilter;
    LogLevel logLevel;

public:
    AreaFilter() : logLevel(LogLevel::TRACE) {}
    bool Enabled(LogLevel level, int areaId) override;
    void SetReportingLevel(LogLevel level) override;
    void SetFilter(int areaId, LogLevel level);
};

}