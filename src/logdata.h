#pragma once
#include <string>
#include "loglevel.h"

namespace asynclog
{

struct Logdata
{
    Timestamp timestamp;
    LogLevel level;
    int areaId;
    std::string message;

    Logdata() : timestamp(0), level(LogLevel::ERROR), areaId(0) {}

    Logdata(LogLevel aseverity) : timestamp(std::time(nullptr)), level(aseverity), areaId(0) {}

    Logdata(LogLevel aseverity, int anAreaId) : timestamp(std::time(nullptr)), level(aseverity), areaId(anAreaId) {}

    Logdata(LogLevel aseverity, int anAreaId, const std::string amessage)
        : timestamp(std::time(nullptr)), level(aseverity), areaId(anAreaId), message(amessage) {}

    Logdata(int anAreaId, const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), areaId(anAreaId), message(amessage) {}

    Logdata(const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), areaId(0), message(amessage) {}

    Logdata(const Logdata& other)
        : timestamp(other.timestamp), level(other.level), areaId(other.areaId), message(other.message) {}

    Logdata& operator=(const Logdata& other) {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            areaId = other.areaId;
            message = other.message;
        }
        return *this;
    }

    Logdata(Logdata&& other) noexcept
        : timestamp(other.timestamp), level(other.level), areaId(other.areaId), message(std::move(other.message))  {}

    Logdata& operator=(Logdata&& other) noexcept {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            areaId = other.areaId;
            message = std::move(other.message);
        }
        return *this;
    }

};

}