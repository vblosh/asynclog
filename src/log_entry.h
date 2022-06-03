#pragma once
#include "interfaces.h"
#include "logger.h"
#include "logdata.h"

#include <sstream>

namespace asynclog
{

class LogEntry final
{
    Logger& log;
    std::ostringstream bufStream;
    Logdata logData;

public:
    LogEntry(Logger& alog, Logdata&& logdata) : log(alog), logData(std::move(logdata)) 
    {}

    ~LogEntry()
    {
        logData.message = bufStream.str();
        log.Log(std::move(logData));
    }

    std::ostream& Get()
    {
        return bufStream;
    }
};

}