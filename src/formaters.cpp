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
    buf << getLogLevelName(logdata.level) << " : ";
    if (logdata.areaId != areas::DEFAULT) {
        buf << '[' << areas::getAreaName(logdata.areaId) << "] ";
    }
    buf << logdata.message << std::endl;
}

static void escapeJsonString(std::ostream& buf, const std::string& str)
{
    for (char c : str) {
        switch (c) {
            case '"':  buf << "\\\""; break;
            case '\\': buf << "\\\\"; break;
            case '\b': buf << "\\b"; break;
            case '\f': buf << "\\f"; break;
            case '\n': buf << "\\n"; break;
            case '\r': buf << "\\r"; break;
            case '\t': buf << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    buf << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
                } else {
                    buf << c;
                }
                break;
        }
    }
}

void JsonFormatter::Format(std::ostream& buf, const Logdata& logdata)
{
    std::tm tm;
#if defined(_MSC_VER)
    localtime_s(&tm, &logdata.timestamp);
#else
    localtime_r(&logdata.timestamp, &tm);
#endif

    buf << "{\"timestamp\":\"" << std::put_time(&tm, timeformat.c_str())
        << "\",\"level\":\"" << getLogLevelName(logdata.level)
        << "\",\"area\":\"" << areas::getAreaName(logdata.areaId)
        << "\",\"message\":\"";
    escapeJsonString(buf, logdata.message);
    buf << "\"}" << std::endl;
}

}