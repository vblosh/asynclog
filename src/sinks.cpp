#include "sinks.h"

namespace simplelogger
{

void SinkComposite::Log(const Logdata& logdata)
{
    for (auto& sink : sinks) {
        sink->Log(logdata);
    }
}

bool SinkComposite::Enabled(const Logdata& logdata)
{
    for (auto& sink : sinks) {
        if (sink->Enabled(logdata)) {
            return true;
        }
    }
    return false;
}

void SinkComposite::SetReportingLevel(LogLevel level)
{
    for (auto& sink : sinks) {
        sink->SetReportingLevel(level);
    }
}

void SinkComposite::AddSink(const FilteredSinkPtr& os)
{
    sinks.push_back(os);
}

void SinkComposite::Clear()
{
    sinks.clear();
}

FilteredSink::FilteredSink(SinkPtr asink, FilterPtr afilter)
    : sink(asink), filter(afilter)
{
}

void FilteredSink::Log(const Logdata& logdata)
{
    sink->Log(logdata);
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