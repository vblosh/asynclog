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

void SinkComposite::AddSink(const SinkPtr& os)
{
    sinks.push_back(os);
}

SinkCout::SinkCout(std::shared_ptr<IFilter> afilter, std::shared_ptr<IFormatter> logformatter)
    : filter(afilter), formatter(logformatter)
{
}

SinkCout::~SinkCout()
{
}

void SinkCout::Log(const Logdata& logdata)
{
    formatter->Format(std::cout, logdata);
}

bool SinkCout::Enabled(const Logdata& logdata)
{
    return filter->Enabled(logdata);
}

SinkFile::SinkFile(const std::string& filename, std::shared_ptr<IFilter> afilter, std::shared_ptr<IFormatter> logformatter)
    : filter(afilter), formatter(logformatter)
{
    ofs.open(filename.c_str(), std::ios::out | std::ios::trunc);
}

SinkFile::~SinkFile()
{
    ofs.close();
}

void SinkFile::Log(const Logdata& logdata)
{
    formatter->Format(ofs, logdata);
}

bool SinkFile::Enabled(const Logdata& logdata)
{
    return filter->Enabled(logdata);
}

}