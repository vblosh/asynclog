#include "filters.h"

namespace asynclog
{

bool AreaFilter::Enabled(LogLevel level, const std::string& area)
{
    auto it = areaFilter.find(area);
    if (it != areaFilter.cend()) {
        return level >= it->second;
    }
    return level >= logLevel;
}

void AreaFilter::SetFilter(const std::string& area, LogLevel level)
{
    areaFilter[area] = level;
}

void AreaFilter::SetReportingLevel(LogLevel level)
{
    logLevel = level;
}

}