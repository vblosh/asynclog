#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <stop_token>
#include <mutex>

#include "sinks.h"
#include "mpscqueue.h"
#include "node.h"

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
    NodeAllocator allocator;
    MpscQueue logQueue;
    SinkPtr sink;
    // Declared last: std::jthread requests stop and joins in its destructor,
    // so the consumer finishes before the queue, allocator and sink above
    // are destroyed. No explicit Stop() needed.
    std::jthread consumer;

public:
    AsyncSink(SinkPtr asink, size_t queueSize = 1024*10);

    void Log(const Logdata&) override;
    void Log(Logdata&& logdata) override;

private:
    void Consume(std::stop_token stoken);
};

}