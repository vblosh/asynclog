#pragma once
#include <memory>
#include "interfaces.h"
#include "sinks.h"

namespace asynclog
{

class Logger
{
    std::vector<FilteredSinkPtr> sinks;

    LogLevel reportingLevel;

public:
    static Logger& Instance()
    {
        static Logger _instance;
        return _instance;
    }

    Logger() : reportingLevel(LogLevel::TRACE)  {}

    void AddSink(const FilteredSinkPtr& os);

    bool Enabled(const LogLevel level) const;

    bool Enabled(const LogLevel level, const std::string& area) const;

    void Log(Logdata&& logdata);

    LogLevel ReportingLevel() const;

    void SetReportingLevel(LogLevel level);

    void Shutdown();
};

}
