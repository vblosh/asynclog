#pragma once 
#include <string>

#include "interfaces.h"

namespace simplelogger
{

class LogFormatter : public IFormatter
{
    std::string timeformat;
    static std::array<std::string, 7> labels;

public:
    LogFormatter(const std::string tmformat = "%d/%m/%Y %H:%M:%S") : timeformat(tmformat) {}

    void Format(std::ostream& buf, const Logdata& logdata) override;
};

}