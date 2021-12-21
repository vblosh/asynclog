#ifndef GUARD_LOGGING_H
#define GUARD_LOGGING_H

#include "interfaces.h"
#include "formaters.h"
#include "sinks.h"
#include "logger.h"
#include "log_entry.h"

#ifndef LOG_MAX_LEVEL
#ifdef NDEBUG
#define LOG_MAX_LEVEL LogLevel::INFO
#else
#define LOG_MAX_LEVEL LogLevel::TRACE
#endif // DEBUG
#endif // LOG_MAX_LEVEL

#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL LogLevel::INFO
#endif // !DEFAULT_LOG_LEVEL

#define LOG(level, ...) \
if (level < LOG_MAX_LEVEL) ;\
else if( !::asynclog::Logger::Instance().Enabled(level, __VA_ARGS__) ) ; \
else ::asynclog::LogEntry(::asynclog::Logger::Instance(), Logdata(level, __VA_ARGS__)).Get()

#define SLOG(level, ...) \
if (level < LOG_MAX_LEVEL) ;\
else ::asynclog::Logger::Instance().Log(Logdata(__VA_ARGS__, level))


#endif //GUARD_LOGGING_H