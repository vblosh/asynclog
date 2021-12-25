#pragma once
#include <iostream>
#include <fstream>
#include "sinks.h"
#include "mpscqueue.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace asynclog
{

class FormattedStreamSink : public ISink
{
protected:
    FormatterPtr formatter;
    std::ostream* os;
    std::mutex mutex;

public:
    FormattedStreamSink(FormatterPtr aformatter);

    void Log(const Logdata& logdata) override;
    void Log(Logdata&& logdata) override;
};

// Discards all log messages
struct SinkNull : public ISink
{
    void Log(const Logdata&) override {}
    void Log(Logdata&& logdata) override {}
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

class AsyncSink : public ISink
{
    std::exception_ptr thread_exception_ptr;
    MpscQueue logQueue;
    std::thread consumer;
    std::atomic<bool> proceed;
    SinkPtr sink;

public:
    AsyncSink(SinkPtr asink, size_t queueSize = 1024*10);
    ~AsyncSink();

    void Log(const Logdata&) override;
    void Log(Logdata&& logdata) override;

private:
    void Start();
    void Stop();
    void Consume();
};

}