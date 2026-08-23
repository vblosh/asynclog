#pragma once
#include <string>
#include "loglevel.h"

namespace asynclog
{

struct Logdata
{
    Timestamp timestamp;
    LogLevel level;
    std::string area;
    std::string message;

    Logdata() : timestamp(0), level(LogLevel::ERROR) {}

    Logdata(LogLevel aseverity) : timestamp(std::time(nullptr)), level(aseverity) {}

    Logdata(LogLevel aseverity, const std::string& anarea) : timestamp(std::time(nullptr)), level(aseverity), area(anarea) {}

    Logdata(LogLevel aseverity, const std::string anarea, const std::string amessage)
        : timestamp(std::time(nullptr)), level(aseverity), area(anarea), message(amessage) {}

    Logdata(const std::string anarea, const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), area(anarea), message(amessage) {}

    Logdata(const std::string amessage, LogLevel aseverity)
        : timestamp(std::time(nullptr)), level(aseverity), message(amessage) {}

    Logdata(const Logdata& other)
        : timestamp(other.timestamp), level(other.level), area(other.area), message(other.message) {}

    Logdata& operator=(const Logdata& other) {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            area = other.area;
            message = other.message;
        }
        return *this;
    }

    Logdata(Logdata&& other) noexcept
        : timestamp(other.timestamp), level(other.level), area(std::move(other.area)), message(std::move(other.message))  {}

    Logdata& operator=(Logdata&& other) noexcept {
        if (&other != this) {
            timestamp = other.timestamp;
            level = other.level;
            area = std::move(other.area);
            message = std::move(other.message);
        }
        return *this;
    }

};

}