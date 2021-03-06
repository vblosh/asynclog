#include "logger.h"

namespace asynclog
{
void Logger::AddSink(const FilteredSinkPtr& os)
{
    sinks.push_back(os);
}

bool Logger::Enabled(const LogLevel level) const
{
    return !(level < reportingLevel || sinks.empty());
}

bool Logger::Enabled(const LogLevel level, const std::string& area) const
{
    for (size_t i = 0; i < sinks.size(); ++i) {
        if (sinks[i]->Enabled(level, area)) {
            return true;
        }
    }
    return Enabled(level);
}

void Logger::Log(Logdata&& logdata) {
    if (sinks.empty())
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
    for (size_t i = 0; i < sinks.size(); ++i) {
        sinks[i]->SetReportingLevel(level);
    }
}

void Logger::Shutdown()
{
    sinks.clear();
}

}
