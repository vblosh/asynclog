# asynclog

A lightweight, thread-safe asynchronous logging library for C++14.

`asynclog` routes log messages from any number of producer threads to one or
more sinks through a lock-free MPSC (multi-producer single-consumer) queue, so
formatting and I/O happen on a background thread instead of the hot path of
your application.

## Features

- **Asynchronous sinks** — producers only enqueue a log record; a consumer
  thread performs formatting and writing.
- **Stream-style and string-style macros** — `LOG(level, area) << ...` with
  lazy evaluation (the message is never formatted when it would be discarded),
  and `SLOG(level, area, message)` for pre-built strings.
- **Two-level filtering** — a global reporting level plus per-area level
  overrides (`AreaFilter`), and compile-time elimination of log statements
  below `LOG_MAX_LEVEL`.
- **Multiple sinks** — console, file, null, or your own via the `ISink`
  interface; each sink can be wrapped in `AsyncSink` and have its own filter.
- **Pluggable formatting** — implement `IFormatter` or use the built-in
  `LogFormatter`.
- **No external dependencies** for the library itself (STL only).

## Requirements

- CMake 3.8+
- A C++14 compiler (GCC, Clang, MSVC)
- GoogleTest (unit tests only)
- log4cplus (optional `perfcompare` benchmark only)

## Building

```sh
mkdir build && cd build
cmake ..
cmake --build .
```

This builds:

| Target         | Description                                  |
|----------------|----------------------------------------------|
| `asynclog`     | Static library (`src/`)                      |
| `runUnitTests` | GoogleTest unit tests (`tests/`)             |
| `perftests`    | Multithreaded latency benchmark (`perftests/`)|

Run the tests:

```sh
ctest
# or directly
./tests/runUnitTests
```

The `perfcompare` sub-project (benchmark against log4cplus) is disabled by
default; uncomment `add_subdirectory ("perfcompare")` in the top-level
`CMakeLists.txt` to enable it (requires log4cplus).

## Quick start

```cpp
#include "logging.h"
#include "sinksimp.h"

using namespace asynclog;

int main()
{
    // Add an asynchronous file sink.
    Logger::Instance().AddSink(
        FilteredSinkPtr(new FilteredSink(
            SinkPtr(new AsyncSink(SinkPtr(new SinkFile("app.log")))))));

    // Add a synchronous console sink.
    Logger::Instance().AddSink(
        FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkCout))));

    Logger::Instance().SetReportingLevel(LogLevel::INFO);

    LOG(LogLevel::INFO, "NET") << "listening on port " << 8080;
    SLOG(LogLevel::ERROR, "DB", "connection failed");

    // Stop all logging threads first, then:
    Logger::Instance().Shutdown();
}
```

Output:

```
22/08/2026 12:00:00 INFO  : [NET] listening on port 8080
22/08/2026 12:00:00 ERROR : [DB] connection failed
```

## Usage

### Log levels

`LogLevel` (in `loglevel.h`), from lowest to highest severity:

```
TRACE, DEBUG, INFO, NOTICE, WARNING, ERROR, FATAL, NONE
```

A message is logged when its level is **greater than or equal to** the
effective reporting level. `NONE` disables everything.

### Logging macros

Defined in `logging.h`:

```cpp
LOG(level, area)  << "stream " << "message";  // with area
LOG(level)        << "stream message";        // without area
SLOG(level, area, message);                   // string message with area
SLOG(level, message);                         // string message without area
```

`LOG` builds the message lazily: if no sink will accept the record, the
stream expression is never evaluated, so filtered-out logging is almost free.
`SLOG` takes an already-built `std::string`.

### Compile-time filtering

Define `LOG_MAX_LEVEL` before including `logging.h` (or via your build
system) to strip out all statements below that level at compile time:

```cpp
#define LOG_MAX_LEVEL LogLevel::WARNING
#include "logging.h"
```

Defaults: `LogLevel::TRACE` in debug builds, `LogLevel::INFO` when `NDEBUG`
is defined.

### Runtime filtering

```cpp
// Global reporting level (propagated to all sinks' filters):
Logger::Instance().SetReportingLevel(LogLevel::ERROR);

// Per-area override on a specific sink's AreaFilter:
std::shared_ptr<AreaFilter> filter(new AreaFilter);
filter->SetFilter("NOISY_COMPONENT", LogLevel::ERROR);
filter->SetFilter("IMPORTANT", LogLevel::TRACE); // kept even above global level

Logger::Instance().AddSink(
    FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkCout), filter)));
```

An explicit per-area filter entry takes precedence over the global reporting
level, so you can silence one noisy area or keep one important area enabled
while the rest is filtered.

### Sinks

All sinks live in `sinksimp.h`:

| Sink          | Description                                              |
|---------------|----------------------------------------------------------|
| `SinkCout`    | Formatted output to `std::cout`.                         |
| `SinkFile`    | Formatted output to a file (truncated on open).          |
| `SinkNull`    | Discards everything.                                     |
| `AsyncSink`   | Decorator: enqueues records and forwards them to the wrapped sink from a dedicated consumer thread. |

`AsyncSink` takes an optional queue size (default `1024 * 10`):

```cpp
SinkPtr async = SinkPtr(new AsyncSink(SinkPtr(new SinkFile("app.log")), 4096));
```

### Custom sinks and formatters

Implement the interfaces from `interfaces.h`:

```cpp
struct ISink {
    virtual void Log(const Logdata& logdata) = 0;
    virtual void Log(Logdata&& logdata) = 0;
};

struct IFormatter {
    virtual void Format(std::ostream& buf, const Logdata& logdata) = 0;
};

struct IFilter {
    virtual bool Enabled(LogLevel level, const std::string& area) = 0;
    virtual void SetReportingLevel(LogLevel level) = 0;
};
```

`Logdata` carries the `timestamp`, `level`, `area` and `message` of a record.
`LogFormatter` accepts a `strftime`-style time format
(default `"%d/%m/%Y %H:%M:%S"`).

## Threading contract

- **Logging** (`LOG`, `SLOG`, `Logger::Log`, `Logger::Enabled`) may be called
  from any number of threads concurrently.
- **Configuration** (`AddSink`, `SetReportingLevel`, `Shutdown`) must be done
  from a single controlling thread and must not run concurrently with
  logging.
- All logging threads must be stopped before calling
  `Logger::Instance().Shutdown()` — destroying an `AsyncSink` flushes its
  queue and joins its consumer thread.

## Project structure

```
src/          the asynclog static library
  logging.h     LOG / SLOG macros — the main include
  logger.*      Logger singleton
  logdata.h     Logdata record, loglevel.h LogLevel enum
  interfaces.h  ISink / IFilter / IFilteredSink / IFormatter
  sinks.*       FilteredSink (sink + filter)
  sinksimp.*    SinkCout, SinkFile, SinkNull, AsyncSink
  filters.*     AreaFilter (per-area levels)
  formaters.*   LogFormatter
  mpscqueue.h   lock-free MPSC queue
  node.h        Node + chunked NodeAllocator
  spinlock.h    spinlock used by the allocator
tests/        GoogleTest unit tests
perftests/    latency benchmark (min/median/max per thread)
perfcompare/  benchmark against log4cplus (disabled by default)
```

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE)
file for details.
