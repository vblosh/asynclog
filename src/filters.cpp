#include "filters.h"

namespace simplelogger
{

bool AreaFilter::Enabled(const Logdata& data)
{
    auto it = areaFilter.find(data.area);
    if (it != areaFilter.cend()) {
        return data.severity >= it->second;
    }
    return true;
}

void AreaFilter::SetFilter(const std::string& area, LogLevel level)
{
    areaFilter[area] = level;
}

}