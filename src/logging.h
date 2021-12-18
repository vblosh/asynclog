#ifndef GUARD_LOGGING_H
#define GUARD_LOGGING_H

#include "interfaces.h"
#include "formaters.h"
#include "sinks.h"
#include "log_settings.h"
#include "log_entry.h"

//#define LOG(level) \
//    if (!::asynclog::LogSettings::Instance().GetSink()->Enabled(Logdata(std::time(0), level, ""))) ; \
//    else ::asynclog::LogEntry(::asynclog::LogSettings::Instance().GetSink(), Logdata(std::time(nullptr), level, "")).Get()

#define LOG(level, area) \
    if (!::asynclog::LogSettings::Instance().GetSink()->Enabled(Logdata(std::time(0), level, area))) ; \
    else ::asynclog::LogEntry(::asynclog::LogSettings::Instance().GetSink(), Logdata(std::time(nullptr), level, area)).Get()

//#define SLOG(level, message) \
//    if (!::asynclog::LogSettings::Instance().GetSink()->Enabled(Logdata(std::time(0), level, ""))) ; \
//    else ::asynclog::LogSettings::Instance().GetSink()->Log(Logdata(std::time(nullptr), level, "", message))

#define SLOG(level, area, message) \
    if (!::asynclog::LogSettings::Instance().GetSink()->Enabled(Logdata(std::time(0), level, area))) ; \
    else ::asynclog::LogSettings::Instance().GetSink()->Log(Logdata(std::time(nullptr), level, area, message))

#endif //GUARD_LOGGING_H