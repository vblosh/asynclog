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

void SinkComposite::AddSink(const FilteredSinkPtr& os)
{
    sinks.push_back(os);
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

}