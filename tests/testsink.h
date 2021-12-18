#pragma once
#include "interfaces.h"

using namespace asynclog;

class TestSink : public ISink
{
	Logdata lastEntry;
	unsigned int count;

public:
	TestSink() : lastEntry(0, LogLevel::NONE), count(0) {}
	~TestSink() {}

	// Inherited via ISink
	virtual void Log(const Logdata& logdata) override;

	const Logdata& LastEntry() const { return lastEntry; }
	unsigned int Count() const { return count; }
};

