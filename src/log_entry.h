#pragma once
#include "interfaces.h"

#include <sstream>

namespace simplelogger
{

class LogEntry final
{
    ISink* pStream;
    std::ostringstream bufStream;
    Logdata logData;

public:
    LogEntry(ISink* pSink, const Logdata& logdata) : pStream(pSink), logData(logdata) {}

    ~LogEntry()
    {
        logData.message = bufStream.str();
        pStream->Log(logData);
    }

    std::ostream& Get()
    {
        return bufStream;
    }
};

}