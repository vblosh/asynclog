// testNullstream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//
#include <iostream>
#include <fstream>
#include <chrono>
#include <gtest/gtest.h>

#include "logging.h"
#include "sinksimp.h"
#include "testsink.h"

using namespace std;
using namespace simplelogger;

TEST(simpleLogger, testSink1)
{
	std::string area = "AREA1";
	string message = "Hello, world!";
	TestSink* testSink = new TestSink;
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(testSink))));
	LogSettings::Instance().SetReportingLevel(LogLevel::ERROR);

	Timestamp before = std::time(nullptr);
	LOG(LogLevel::ERROR, area) << message;
	Timestamp after = std::time(nullptr);

	ASSERT_EQ(1, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().severity);
	ASSERT_EQ(area, testSink->LastEntry().area);
	ASSERT_EQ(message, testSink->LastEntry().message);

	LogSettings::Instance().Shutdown();
}

bool LogAndCheck(LogLevel level, const std::string& area, const std::string& message, TestSink* testSink)
{
	unsigned int oldCount = testSink->Count();
	Timestamp before = std::time(nullptr);
	LOG(LogLevel::ERROR, area) << message;
	Timestamp after = std::time(nullptr);

	bool res = true;
	res &= testSink->Count() == oldCount + 1;
	res &= std::difftime(after, before) >= std::difftime(testSink->LastEntry().timestamp, before);
	res &= LogLevel::ERROR == testSink->LastEntry().severity;
	res &= area == testSink->LastEntry().area;
	res &= message == testSink->LastEntry().message;
	return res;
}

TEST(simpleLogger, testSink2)
{
	std::string area = "AREA1";
	string message = "Hello, world!";
	
	std::shared_ptr<TestSink> testSink1(new TestSink);
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink1)));
	std::shared_ptr<TestSink> testSink2(new TestSink);
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink2)));
	LogSettings::Instance().SetReportingLevel(LogLevel::ERROR);

	ASSERT_TRUE(LogAndCheck(LogLevel::ERROR, area, message, testSink1.get()));
	ASSERT_TRUE(LogAndCheck(LogLevel::ERROR, area, message, testSink2.get()));

	LogSettings::Instance().Shutdown();
}

TEST(simpleLogger, testReportingLevel)
{
	std::string area = "AREA1";
	string message = "Hello, world!";
	
	std::shared_ptr<TestSink> testSink(new TestSink);
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink)));
	LogSettings::Instance().SetReportingLevel(LogLevel::ERROR);

	// LogLevel is equal ReportingLevel, message logged
	LOG(LogLevel::ERROR, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is less than ReportingLevel, message not logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is greater than ReportingLevel, message logged
	LOG(LogLevel::FATAL, area) << message;
	ASSERT_EQ(2, testSink->Count());

	LogSettings::Instance().Shutdown();
}

TEST(simpleLogger, testFilter)
{
	std::string area = "AREA1";
	string message = "Hello, world!";

	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(area, LogLevel::WARNING);

	std::shared_ptr<TestSink> testSink(new TestSink);
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink, filter)));
	LogSettings::Instance().SetReportingLevel(LogLevel::TRACE);

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	ASSERT_EQ(1, testSink->Count());

	LogSettings::Instance().SetReportingLevel(LogLevel::NONE);

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(2, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	ASSERT_EQ(2, testSink->Count());

	LogSettings::Instance().Shutdown();
}

