#pragma once
#include <iostream>
#include <fstream>
#include "sinks.h"
#include "circularbuffer.h"

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

class AsyncSink : public ISink
{
    circullar_buffer<Logdata> buffer;
    std::thread consumer;
    std::mutex buffer_mutex;
    std::atomic<bool> proceed;
    std::condition_variable enqueue_condition_var;
    std::exception_ptr thread_exception_ptr = nullptr;
    
    bool started;
    std::condition_variable started_condition_var;
    std::mutex started_mutex;

    SinkPtr sink;

public:
    AsyncSink(size_t bufferSize, SinkPtr asink);
    ~AsyncSink();

    void Log(const Logdata&) override;

private:
    void Start();
    void Stop();
    void Consume();
};

}