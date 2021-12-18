#pragma once

#include <string>
#include <ctime>
#include <iostream>

namespace asynclog
{

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

using Timestamp = std::time_t;

struct Logdata
{
    Timestamp timestamp;
    LogLevel severity;
    std::string area;
    std::string message;

    Logdata() : timestamp(0), severity(LogLevel::ERROR) {}

    Logdata(Timestamp theTimestamp, LogLevel aseverity, const std::string& anarea = std::string(), const std::string& amessage = std::string())
        : timestamp(theTimestamp), severity(aseverity), area(anarea), message(amessage) {}
};

//log sink interface
struct ISink
{
    virtual ~ISink() = default;
    virtual void Log(const Logdata& logdata) = 0;
};

// sink filter interface
// make possible to enable/disable sink based on logdata
struct IFilter
{
    virtual ~IFilter() = default;
    virtual bool Enabled(const Logdata& logdata) = 0;
    virtual void SetReportingLevel(LogLevel level) = 0;
};

// sink with filter interface
//All log sinks must inherit from this 
struct IFilteredSink : public ISink, public IFilter
{
    virtual ~IFilteredSink() = default;
};

// Formatter interface
struct IFormatter
{
    virtual ~IFormatter() = default;
    virtual void Format(std::ostream& buf, const Logdata& logdata) = 0;
};
}