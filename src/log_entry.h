#pragma once
#include "interfaces.h"

#include <sstream>

namespace asynclog
{

class LogEntry final
{
    ISink* pSink;
    std::ostringstream bufStream;
    Logdata logData;

public:
    LogEntry(ISink* pSink, Logdata&& logdata) : pSink(pSink), logData(std::forward<Logdata>(logdata)) {}

    ~LogEntry()
    {
        logData.message = bufStream.str();
        pSink->Log(logData);
    }

    std::ostream& Get()
    {
        return bufStream;
    }
};

}