#pragma once
#include <atomic>
#include <memory>
#include "interfaces.h"
#include "sinks.h"

namespace asynclog
{

// Threading contract: logging (LOG/SLOG/Log/Enabled) may run on any number
// of threads concurrently. Configuration methods (AddSink, SetReportingLevel,
// Shutdown) must be called from a single controlling thread and must not run
// concurrently with logging; in particular all logging threads must be
// stopped before Shutdown().
class Logger
{
    std::vector<FilteredSinkPtr> sinks;

    std::atomic<LogLevel> reportingLevel;

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
