#pragma once
#include <memory>
#include "interfaces.h"
#include "sinks.h"

namespace simplelogger
{

class LogSettings
{
    std::unique_ptr<SinkComposite> sinks;
    LogLevel reportingLevel;

public:
    static LogSettings& Instance()
    {
        static LogSettings _instance;
        return _instance;
    }

    LogSettings() : sinks(new SinkComposite), reportingLevel(LogLevel::ERROR)
    {}

    void AddSink(const FilteredSinkPtr& os)
    {
        sinks->AddSink(os);
    }

    IFilteredSink* GetSink()
    {
        return sinks.get();
    }

    LogLevel& ReportingLevel()
    {
        return reportingLevel;
    }

    void SetReportingLevel(const LogLevel& level)
    {
        reportingLevel = level;
    }

    void Shutdown()
    {
        sinks = nullptr;
    }
};

}
