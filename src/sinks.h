#pragma once
#include <iostream>
#include <fstream>

#include "filters.h"
#include "formaters.h"

namespace simplelogger
{

using  SinkPtr=std::shared_ptr<IFilteredSink>;

// composite pattern, make possible to use many sinks
class SinkComposite : public IFilteredSink
{
    std::vector<SinkPtr> sinks;

public:
    void Log(const Logdata& logdata) override;

    bool Enabled(const Logdata& logdata) override;

    void AddSink(const SinkPtr& os);
};

// Discards all log messages
struct SinkNull : IFilteredSink
{
    void Log(const Logdata&) override {}
    bool Enabled(const Logdata& logdata) override { return false; }
};

class SinkCout : public IFilteredSink
{
    std::shared_ptr<IFilter> filter;
    std::shared_ptr<IFormatter> formatter;

public:

    SinkCout(std::shared_ptr<IFilter> afilter = std::shared_ptr<IFilter>(new AreaFilter())
        , std::shared_ptr<IFormatter> logformatter = std::shared_ptr<IFormatter>(new LogFormatter()));

    ~SinkCout() override;

    void Log(const Logdata& logdata) override;

    bool Enabled(const Logdata& logdata) override;
};

class SinkFile : public IFilteredSink
{
    std::ofstream ofs;
    std::shared_ptr<IFilter> filter;
    std::shared_ptr<IFormatter> formatter;

public:

    SinkFile(const std::string& filename, std::shared_ptr<IFilter> afilter = std::shared_ptr<IFilter>(new AreaFilter())
        , std::shared_ptr<IFormatter> logformatter = std::shared_ptr<IFormatter>(new LogFormatter()));

    ~SinkFile() override;

    void Log(const Logdata& logdata) override;

    bool Enabled(const Logdata& logdata) override;
};

}