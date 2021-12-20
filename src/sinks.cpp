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
    sink->Log(std::forward<Logdata>(logdata));
}

bool FilteredSink::Enabled(const Logdata& logdata)
{
    return filter->Enabled(logdata);
}

void FilteredSink::SetReportingLevel(LogLevel level)
{
    filter->SetReportingLevel(level);
}



}