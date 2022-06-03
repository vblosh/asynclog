#pragma once
#include <ctime>

namespace asynclog
{
using Timestamp = std::time_t;

enum class LogLevel
{
    TRACE = 0,
    DEBUG,
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    FATAL,
    NONE
};

}


