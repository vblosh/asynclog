#include "logger.h"

namespace asynclog
{
void Logger::AddSink(const FilteredSinkPtr& os)
{
    sinks.push_back(os);
}

void Logger::Log(Logdata&& logdata) {
    if (logdata.level < reportingLevel || sinks.empty())
        return;

    for (size_t i = 1; i < sinks.size(); i++) {
        if (sinks[i]->Enabled(logdata)) {
            sinks[i]->Log(logdata);
        }
    }

    if (sinks[0]->Enabled(logdata)) {
        sinks[0]->Log(std::move(logdata));
    }
    }

LogLevel& Logger::ReportingLevel()
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
