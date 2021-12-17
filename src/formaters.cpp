#include <iomanip>
#include <ctime>
#include <array>

#include "formaters.h"

namespace simplelogger 
{
std::array<std::string, 7> LogFormatter::labels{ "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL", "NONE " };

void LogFormatter::Format(std::ostream& buf, const Logdata& logdata)
{
    std::tm tm;
#if defined(__unix__)
    localtime_r(&logdata.timestamp, &tm);
#elif defined(_MSC_VER)
    localtime_s(&tm, &logdata.timestamp);
#else
    tm = *std::localtime(&timer);
#endif

    buf << std::put_time(&tm, timeformat.c_str()) << ' ';
    buf << labels[(unsigned char)logdata.severity] << " : ";
    if (!logdata.area.empty()) {
        buf << '[' << logdata.area << "] ";
    }
    buf << logdata.message;
}

}