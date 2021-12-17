#ifndef GUARD_LOGGING_H
#define GUARD_LOGGING_H

#include "interfaces.h"
#include "formaters.h"
#include "sinks.h"
#include "log_settings.h"
#include "log_entry.h"

// Takes 1-2 arguments, a loglevel and a log area (defaults to "" if only 1 argument is given). Do not call with more arguments, the result is an unintuitive compile error.
#define LOG(level, ...) \
    if (!::simplelogger::LogSettings::Instance().GetSink()->Enabled(Logdata(std::time(0), level, __VA_ARGS__))) ; \
    else ::simplelogger::LogEntry(::simplelogger::LogSettings::Instance().GetSink(), Logdata(std::time(nullptr), level, __VA_ARGS__)).Get()

#endif //GUARD_LOGGING_H