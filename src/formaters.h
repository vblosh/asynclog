#pragma once 
#include <array>
#include <string>
#include <string_view>

#include "interfaces.h"

namespace asynclog
{

inline constexpr std::array<std::string_view, 8> LogLevelLabels = {
    "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "FATAL", "NONE"
};

inline std::string_view getLogLevelName(LogLevel level) {
    auto idx = static_cast<size_t>(level);
    if (idx < LogLevelLabels.size()) {
        return LogLevelLabels[idx];
    }
    return "UNKNOWN";
}

class LogFormatter : public IFormatter
{
    std::string timeformat;

public:
    LogFormatter(const std::string tmformat = "%d/%m/%Y %H:%M:%S") : timeformat(tmformat) {}

    void Format(std::ostream& buf, const Logdata& logdata) override;
};

class JsonFormatter : public IFormatter
{
    std::string timeformat;

public:
    JsonFormatter(const std::string tmformat = "%Y-%m-%d %H:%M:%S") : timeformat(tmformat) {}

    void Format(std::ostream& buf, const Logdata& logdata) override;
};

}