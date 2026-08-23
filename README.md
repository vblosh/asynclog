# asynclog

A lightweight, thread-safe asynchronous logging library for C++20.

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
- **Multiple sinks** — console, file, null, composite, or your own via the `ISink`
  interface; each sink can be wrapped in `AsyncSink` and have its own filter.
- **Pluggable formatting** — implement `IFormatter` or use the built-in
  `LogFormatter` or `JsonFormatter`.
- **No external dependencies** for the library itself (STL only).

## Requirements

- CMake 3.8+
- A C++20 compiler (GCC 10+, Clang 14+, MSVC 2019 16.9+)
- GoogleTest (unit tests only)
- log4cplus (optional `perfcompare` benchmark only)

The library takes advantage of modern C++: `std::jthread` /
`std::stop_token` drive the async consumer thread (no manual stop flag or
join), the `LOG` macro uses standard `__VA_OPT__` instead of the
`##__VA_ARGS__` compiler extension, plus C++17 `inline static` members,
`std::scoped_lock` and `[[nodiscard]]`.

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

## Performance

The `perfcompare` benchmark measures logging latency under concurrent load
(10 threads, 1000 iterations per thread, 10:1 filtered-to-logged ratio).

### Benchmark Results

| Metric | asynclog | log4cplus | Speedup |
|--------|----------|-----------|---------|
| **Logged messages (median)** | 1.70 µs | 377.79 µs | **222x faster** |
| **Logged messages (avg mean)** | 1.67 µs | 362.57 µs | **217x faster** |
| **Filtered messages (median)** | 0.02 µs | 0.03 µs | ~1.5x faster |

### Key Insights

**Logged messages**: asynclog's asynchronous design with a lock-free MPSC queue
provides dramatically lower latency. Producers only enqueue the log record and
return immediately, while a background thread handles formatting and I/O. This
eliminates contention and blocking on file/console writes.

**Filtered messages**: Both libraries perform similarly for filtered messages
that don't reach any sink. asynclog uses compile-time elimination
(`LOG_MAX_LEVEL`) and integer-based area filtering for minimal overhead.

**Why asynclog wins**:
- Lock-free MPSC queue eliminates mutex contention
- Asynchronous I/O keeps logging off the critical path
- Integer area IDs provide O(1) filter lookups (no hash map overhead)
- Compile-time log level elimination removes disabled logs entirely
- Chunked pool allocator (`NodeAllocator`) pre-reserves nodes in contiguous
  memory and recycles deallocated nodes via a free list first before allocating
  new ones, avoiding per-message heap allocations on the hot path

Run the benchmark yourself:
```sh
cd build
./perfcompare/perfcompare
```

## Quick start

```cpp
#include "logging.h"
#include "sinksimp.h"
#include "log_areas.h"

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

    LOG(LogLevel::INFO, areas::NETWORK) << "listening on port " << 8080;
    SLOG(LogLevel::ERROR, areas::DATABASE, "connection failed");

    // Stop all logging threads first, then:
    Logger::Instance().Shutdown();
}
```

Output:

```
22/08/2026 12:00:00 INFO  : [NETWORK] listening on port 8080
22/08/2026 12:00:00 ERROR : [DATABASE] connection failed
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
filter->SetFilter(areas::NETWORK, LogLevel::ERROR);
filter->SetFilter(areas::DATABASE, LogLevel::TRACE); // kept even above global level

Logger::Instance().AddSink(
    FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkCout), filter)));
```

An explicit per-area filter entry takes precedence over the global reporting
level, so you can silence one noisy area or keep one important area enabled
while the rest is filtered.

### Log areas

Log areas are integer identifiers defined in `log_areas.h` using a centralized
registry. This design provides O(1) lookup performance in the `AreaFilter`,
eliminating hash map overhead and string comparisons.

```cpp
// log_areas.h defines areas as an enum:
namespace asynclog::areas {
    enum AreaId : int {
        DEFAULT = 0,
        NETWORK,
        DATABASE,
        UI,
        AUTH,
        FILE_IO,
        TEST,
        DEBUG,
        PERFORMANCE,
        AREA_COUNT  // Automatically tracks the total count
    };
}
```

**Adding new areas:**

1. Add a new entry to the `AreaId` enum in `log_areas.h` (before `AREA_COUNT`)
2. Add a case to `getAreaName()` for display purposes
3. `AREA_COUNT` automatically updates to reflect the new total

**Important:**
- Area IDs must be unique across the entire application
- Once assigned, do not change an area's ID (breaks log file compatibility)
- New areas should be added at the end to maintain backward compatibility

### Sinks

All sinks live in `sinksimp.h`:

| Sink            | Description                                              |
|-----------------|----------------------------------------------------------|
| `SinkCout`      | Formatted output to `std::cout`.                         |
| `SinkFile`      | Formatted output to a file (truncated on open).          |
| `SinkNull`      | Discards everything.                                     |
| `AsyncSink`     | Decorator: enqueues records and forwards them to the wrapped sink from a dedicated consumer thread. |
| `CompositeSink` | Broadcasts records to multiple child sinks sequentially. |

`AsyncSink` takes an optional queue size (default `1024 * 10`):

```cpp
SinkPtr async = SinkPtr(new AsyncSink(SinkPtr(new SinkFile("app.log")), 4096));
```

#### CompositeSink Example

Combine multiple sinks under a single asynchronous queue and background worker:

```cpp
// Create a composite sink broadcasting to both file and console
auto composite = std::make_shared<CompositeSink>(std::initializer_list<SinkPtr>{
    std::make_shared<SinkFile>("app.log"),
    std::make_shared<SinkCout>()
});

// Wrap the composite sink in AsyncSink so both outputs are written on one background thread
Logger::Instance().AddSink(
    FilteredSinkPtr(new FilteredSink(SinkPtr(new AsyncSink(composite))))
);
```

#### Multi-Sink Logging Strategies

When routing log messages to multiple outputs asynchronously, you have two design options:

1. **Option 1: Single `AsyncSink` with `CompositeSink`** — All child sinks share a single queue, memory pool, and background worker thread (`std::jthread`).
2. **Option 2: Multiple independent `AsyncSink` instances** — Each target sink has its own queue, memory pool, and dedicated background worker thread.

| Feature | Option 1: Composite + 1 `AsyncSink` | Option 2: Multiple `AsyncSink`s |
| :--- | :--- | :--- |
| **Worker Threads** | **1 background thread** (`std::jthread`) | **1 thread per sink** |
| **Memory / Queues** | **1 queue & pool allocator** | Separate queue & pool per sink |
| **I/O Execution** | Sinks execute sequentially on the background thread | Sinks execute fully concurrently in background |
| **Fault Isolation** | A slow sink (e.g. slow disk/network) delays other sinks | A slow sink will **not** block other sinks |

### Formatters

Formatters implement `IFormatter` (in `formaters.h`):

| Formatter       | Output Style | Default Format String |
|-----------------|--------------|-----------------------|
| `LogFormatter`  | Human-readable plain text | `"%d/%m/%Y %H:%M:%S"` |
| `JsonFormatter` | JSON Lines (NDJSON) with escaping | `"%Y-%m-%d %H:%M:%S"` |

#### JsonFormatter Example

```cpp
// Attach JsonFormatter to a file sink
auto jsonFileSink = std::make_shared<SinkFile>(
    "events.json",
    std::make_shared<JsonFormatter>("%Y-%m-%dT%H:%M:%S")
);

Logger::Instance().AddSink(
    FilteredSinkPtr(new FilteredSink(SinkPtr(new AsyncSink(jsonFileSink))))
);

LOG(LogLevel::INFO, areas::NETWORK) << "Client connected from \"192.168.1.10\"";
```

Output:
```json
{"timestamp":"2026-08-23T18:00:00","level":"INFO","area":"NETWORK","message":"Client connected from \"192.168.1.10\""}
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
    virtual bool Enabled(LogLevel level, int areaId) = 0;
    virtual void SetReportingLevel(LogLevel level) = 0;
};
```

`Logdata` carries the `timestamp`, `level`, `areaId` and `message` of a record.
`LogFormatter` and `JsonFormatter` accept standard `strftime`-style time format strings.

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
  sinksimp.*    SinkCout, SinkFile, SinkNull, AsyncSink, CompositeSink
  filters.*     AreaFilter (per-area levels)
  formaters.*   LogFormatter, JsonFormatter
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
