#include "sinksimp.h"

namespace asynclog
{
	
FormattedStreamSink::FormattedStreamSink(FormatterPtr aformatter)
    : formatter(aformatter), os(nullptr)
{
}

void FormattedStreamSink::Log(const Logdata& logdata)
{
    std::lock_guard<std::mutex> lock(mutex);
    formatter->Format(*os, logdata);
}

SinkCout::SinkCout(FormatterPtr aformatter)
    : FormattedStreamSink(aformatter)
{
    os = &std::cout;
}

SinkFile::SinkFile(const std::string& filename, FormatterPtr aformatter)
    : FormattedStreamSink(aformatter)
{
    ofs.open(filename.c_str(), std::ios::out | std::ios::trunc);
    os = &ofs;
}

SinkFile::~SinkFile()
{
    ofs.close();
}

AsyncSink::AsyncSink(size_t bufferSize, SinkPtr asink)
    : buffer(bufferSize), sink(asink), proceed(true), started(false)
{
    Start();
}

AsyncSink::~AsyncSink()
{
    Stop();
}

void AsyncSink::Log(const Logdata& logdata)
{
    if (thread_exception_ptr) {
        std::rethrow_exception(thread_exception_ptr);
    }

    std::unique_lock<std::mutex> lock(buffer_mutex);
    buffer.push_back(logdata);
    enqueue_condition_var.notify_one();
}

void AsyncSink::Start()
{
    proceed = true;
    started = false;
    consumer = std::thread(&AsyncSink::Consume, this);
    // wait for consumer thread
    std::unique_lock<std::mutex> lock(started_mutex);
    while (!started) {
        started_condition_var.wait(lock);
    }
}

void AsyncSink::Stop()
{
    using namespace std::chrono_literals;
    proceed = false;
    enqueue_condition_var.notify_one();
    if (consumer.joinable()) {
        consumer.join();
    }
}

void AsyncSink::Consume()
{
    Logdata next_entry;
    try {
        { // signal start
            std::unique_lock<std::mutex> lock(started_mutex);
            started = true;
            started_condition_var.notify_one();
        }

        for (;;) {
            {
                std::unique_lock<std::mutex> lock(buffer_mutex);
                while (proceed && buffer.empty()) {
                    enqueue_condition_var.wait(lock);
                }

                if (proceed) {
                    next_entry = std::move(buffer.front());
                    buffer.pop_front();
                }
                else { // process buffer to the end 
                    while (!buffer.empty()) {
                        next_entry = std::move(buffer.front());
                        buffer.pop_front();
                        sink->Log(next_entry);
                    }
                    // exit thread
                    break;
                }
            }
            sink->Log(next_entry);
        }
    }
    catch (...) {
        thread_exception_ptr = std::current_exception();
    }
}

}