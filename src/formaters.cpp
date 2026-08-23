#include <iomanip>
#include <ctime>
#include <array>

#include "formaters.h"
#include "log_areas.h"

namespace asynclog 
{

void LogFormatter::Format(std::ostream& buf, const Logdata& logdata)
{
    std::tm tm;
#if defined(_MSC_VER)
    localtime_s(&tm, &logdata.timestamp);
#else
    localtime_r(&logdata.timestamp, &tm);
#endif

    buf << std::put_time(&tm, timeformat.c_str()) << ' ';
    buf << labels[(unsigned char)logdata.level] << " : ";
    if (logdata.areaId != areas::DEFAULT) {
        buf << '[' << areas::getAreaName(logdata.areaId) << "] ";
    }
    buf << logdata.message << std::endl;
}

}