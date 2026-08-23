#include "filters.h"

namespace asynclog
{

bool AreaFilter::Enabled(LogLevel level, int areaId)
{
    if (areaId >= 0 && areaId < static_cast<int>(areaFilter.size())) {
        return level >= areaFilter[areaId];
    }
    return level >= logLevel;
}

void AreaFilter::SetFilter(int areaId, LogLevel level)
{
    if (areaId >= static_cast<int>(areaFilter.size())) {
        areaFilter.resize(areaId + 1, logLevel);
    }
    areaFilter[areaId] = level;
}

void AreaFilter::SetReportingLevel(LogLevel level)
{
    logLevel = level;
}

}