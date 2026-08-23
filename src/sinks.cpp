#include "sinks.h"

namespace asynclog
{

FilteredSink::FilteredSink(SinkPtr asink, FilterPtr afilter)
    : sink(asink), filter(afilter)
{
}

void FilteredSink::Log(const Logdata& logdata)
{
    sink->Log(logdata);
}

void FilteredSink::Log(Logdata&& logdata)
{
    sink->Log(std::move(logdata));
}

bool FilteredSink::Enabled(LogLevel level, const std::string& area)
{
    return filter->Enabled(level, area);
}

void FilteredSink::SetReportingLevel(LogLevel level)
{
    filter->SetReportingLevel(level);
}



}