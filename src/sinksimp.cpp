#include "sinksimp.h"
using namespace std::chrono_literals;

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

void FormattedStreamSink::Log(Logdata&& logdata)
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

AsyncSink::AsyncSink(SinkPtr asink, size_t queueSize)
    : thread_exception_ptr(nullptr), allocator(queueSize)
    , logQueue(allocator.allocate(Node(Logdata())))
    , sink(std::move(asink)), consumer(&AsyncSink::Consume, this)
{
}

void AsyncSink::Log(const Logdata& logdata)
{
    Node* node = allocator.allocate(Node(logdata));
    logQueue.push(node);
}

void AsyncSink::Log(Logdata&& logdata)
{
    Node* node = allocator.allocate(Node(std::move(logdata)));
    logQueue.push(node);
}

void AsyncSink::Consume(std::stop_token stoken)
{
    Node* node;
    try {
        // Drain the queue before exiting: keep consuming after a stop is
        // requested until no nodes are left.
        while (node = logQueue.pop(), !stoken.stop_requested() || node != nullptr)
        {
            while (node != nullptr)
            {
                sink->Log(node->value);
                allocator.deallocate(node);
                node = logQueue.pop();
            }
            std::this_thread::sleep_for(10ms);
        }
    }
    catch (...) {
        thread_exception_ptr = std::current_exception();
    }
}

}
