#pragma once
#include "interfaces.h"

using namespace asynclog;

class TestSink : public ISink
{
	Logdata lastEntry;
	unsigned int count;

public:
	TestSink() : lastEntry(LogLevel::NONE, std::string(), std::string()), count(0) {}
	~TestSink() {}

	// Inherited via ISink
	void Log(const Logdata& logdata) override;
	void Log(Logdata&& logdata) override;

	const Logdata& LastEntry() const { return lastEntry; }
	unsigned int Count() const { return count; }
};

