#pragma once 
#include <array>
#include <string>

#include "interfaces.h"

namespace asynclog
{

class LogFormatter : public IFormatter
{
    std::string timeformat;
    inline static std::array<std::string, 8> labels
        { "TRACE", "DEBUG", "INFO ", "NOTICE", "WARN ", "ERROR", "FATAL", "NONE " };

public:
    LogFormatter(const std::string tmformat = "%d/%m/%Y %H:%M:%S") : timeformat(tmformat) {}

    void Format(std::ostream& buf, const Logdata& logdata) override;
};

}