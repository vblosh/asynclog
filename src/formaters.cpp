#include <iomanip>
#include <ctime>
#include <array>

#include "formaters.h"

namespace asynclog 
{
std::array<std::string, 8> LogFormatter::labels{ "TRACE", "DEBUG", "INFO ", "NOTICE", "WARN ", "ERROR", "FATAL", "NONE " };

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
    if (!logdata.area.empty()) {
        buf << '[' << logdata.area << "] ";
    }
    buf << logdata.message << std::endl;
}

}