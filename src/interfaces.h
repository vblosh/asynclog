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
    LogLevel level;
    std::string area;
    std::string message;

    Logdata() : timestamp(0), level(LogLevel::ERROR) {}

    Logdata(LogLevel aseverity) : timestamp(std::time(nullptr)), level(aseverity) {}
    
    Logdata(LogLevel aseverity, const std::string& anarea) : timestamp(std::time(nullptr)), level(aseverity), area(anarea) {}
   
    Logdata(LogLevel aseverity, const std::string anarea, const std::string amessage)
        : timestamp(std::time(nullptr)), level(aseverity), area(anarea), message(amessage) {}

    Logdata(const std::string anarea, const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), area(anarea), message(amessage) {}

    Logdata(const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), message(amessage) {}

    Logdata(const Logdata& other)
        : timestamp(other.timestamp), level(other.level), area(other.area), message(other.message) {}

    Logdata& operator=(const Logdata& other) {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            area = other.area;
            message = other.message;
        }
        return *this;
    }

    Logdata(Logdata&& other)
        : timestamp(other.timestamp), level(other.level), area(std::move(other.area)), message(std::move(other.message)) {}

    Logdata& operator=(Logdata&& other) {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            area = std::move(other.area);
            message = std::move(other.message);
        }
        return *this;
    }
    
};

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