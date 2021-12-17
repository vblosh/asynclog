#pragma once
#include "sinks.h"
#include <iostream>
#include <fstream>

namespace simplelogger
{

class FormattedStreamSink : public ISink
{
protected:
    FormatterPtr formatter;
    std::ostream* os;

public:
    FormattedStreamSink(FormatterPtr aformatter);

    void Log(const Logdata& logdata) override;
};

// Discards all log messages
struct SinkNull : public ISink
{
    void Log(const Logdata&) override {}
};

class SinkCout : public FormattedStreamSink
{
public:
    SinkCout(FormatterPtr aformatter = FormatterPtr(new LogFormatter()));
};

class SinkFile : public FormattedStreamSink
{
    std::ofstream ofs;

public:

    SinkFile(const std::string& filename, FormatterPtr aformatter = FormatterPtr(new LogFormatter()));

    ~SinkFile();
};

}