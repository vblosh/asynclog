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

bool FilteredSink::Enabled(LogLevel level, int areaId)
{
    return filter->Enabled(level, areaId);
}

void FilteredSink::SetReportingLevel(LogLevel level)
{
    filter->SetReportingLevel(level);
}



}