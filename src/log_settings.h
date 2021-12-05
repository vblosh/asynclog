#pragma once
#include "interfaces.h"
#include "sinks.h"

namespace simplelogger
{

class LogSettings
{
    SinkComposite sinks;
    LogLevel reportingLevel;

public:
    static LogSettings& Instance()
    {
        static LogSettings _instance;
        return _instance;
    }

    LogSettings() : reportingLevel(LogLevel::ERROR)
    {}

    void AddSink(const SinkPtr& os)
    {
        sinks.AddSink(os);
    }

    IFilteredSink* GetSink()
    {
        return &sinks;
    }

    LogLevel& ReportingLevel()
    {
        return reportingLevel;
    }

    void SetReportingLevel(const LogLevel& level)
    {
        reportingLevel = level;
    }
};

}
