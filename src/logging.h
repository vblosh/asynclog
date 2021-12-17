#ifndef GUARD_LOGGING_H
#define GUARD_LOGGING_H

#include "interfaces.h"
#include "formaters.h"
#include "sinks.h"
#include "log_settings.h"
#include "log_entry.h"

#ifndef LOG_MAX_LEVEL
#ifdef NDEBUG
#define LOG_MAX_LEVEL LogLevel::WARNING
#else
#define LOG_MAX_LEVEL LogLevel::TRACE
#endif // DEBUG
#endif // LOG_MAX_LEVEL

// Takes 1-2 arguments, a loglevel and a log area (defaults to "" if only 1 argument is given). Do not call with more arguments, the result is an unintuitive compile error.
#define LOG(level, ...) \
if (level < LOG_MAX_LEVEL) ;\
    else if (!::simplelogger::LogSettings::Instance().GetSink()->Enabled(Logdata(level, __VA_ARGS__))) ; \
    else ::simplelogger::LogEntry(::simplelogger::LogSettings::Instance().GetSink(), Logdata(level, __VA_ARGS__)).Get()

#endif //GUARD_LOGGING_H