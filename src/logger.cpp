#include "logger.h"

namespace asynclog
{
void Logger::AddSink(const FilteredSinkPtr& os)
{
    sinks.push_back(os);
}

bool Logger::Enabled(LogLevel level, const std::string& area) const
{
    if (level < reportingLevel || sinks.empty())
        return false;

    for (size_t i = 0; i < sinks.size(); ++i) {
        if (sinks[i]->Enabled(level, area)) {
            return true;
        }
    }
    return false;
}

void Logger::Log(Logdata&& logdata) {
    if (logdata.level < reportingLevel || sinks.empty())
        return;

    for (size_t i = 1; i < sinks.size(); ++i) {
        if (sinks[i]->Enabled(logdata.level, logdata.area)) {
            sinks[i]->Log(logdata);
        }
    }

    if (sinks[0]->Enabled(logdata.level, logdata.area)) {
        sinks[0]->Log(std::move(logdata));
    }
}

LogLevel Logger::ReportingLevel() const
{
    return reportingLevel;
}

void Logger::SetReportingLevel(LogLevel level)
{
    reportingLevel = level;
}

void Logger::Shutdown()
{
    sinks.clear();
}

}
