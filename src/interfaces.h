#pragma once
#include <iostream>
#include "logdata.h"

namespace asynclog
{

//log sink interface
struct ISink
{
    virtual ~ISink() = default;
    virtual void Log(const Logdata& logdata) = 0;
    virtual void Log(Logdata&& logdata) = 0;
};

// sink filter interface
// make possible to enable/disable sink based on logdata
struct IFilter
{
    virtual ~IFilter() = default;
    virtual bool Enabled(LogLevel level, const std::string& area) = 0;
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