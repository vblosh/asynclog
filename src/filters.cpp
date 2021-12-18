#include "filters.h"

namespace asynclog
{

bool AreaFilter::Enabled(const Logdata& data)
{
    auto it = areaFilter.find(data.area);
    if (it != areaFilter.cend()) {
        return data.severity >= it->second;
    }
    return data.severity >= logLevel;
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