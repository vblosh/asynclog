#pragma once

#include <string>
#include <ctime>
#include <iostream>

namespace simplelogger
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

struct Timestamp
{
    time_t timepoint;
    Timestamp(time_t tm) : timepoint(tm) {}

    static time_t now()
    {
        return std::time(nullptr);
    }
};

struct Logdata
{

    Timestamp timestamp;
    LogLevel severity;
    std::string area;
    std::string message;

    Logdata(const LogLevel& aseverity, const std::string& anarea = "")
        : severity(aseverity), area(anarea), timestamp(Timestamp::now()) {}
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