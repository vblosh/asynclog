#include <iostream>
#include <fstream>
#include <chrono>
#include <gtest/gtest.h>

#include "logging.h"
#include "sinksimp.h"
#include "testsink.h"

using namespace std;
using namespace asynclog;

class LoggerTest : public ::testing::Test {
protected:
	std::string area = "AREA1";
	string message = "Hello, world!";

	void SetUp() override {
	}

	void TearDown() override {
		Logger::Instance().Shutdown();
	}

	bool LogAndCheck(LogLevel level, const std::string& area, const std::string& message, TestSink* testSink) {
		unsigned int oldCount = testSink->Count();
		Timestamp before = std::time(nullptr);
		LOG(LogLevel::ERROR, area) << message;
		Timestamp after = std::time(nullptr);

		bool res = true;
		res &= testSink->Count() == oldCount + 1;
		res &= std::difftime(after, before) >= std::difftime(testSink->LastEntry().timestamp, before);
		res &= LogLevel::ERROR == testSink->LastEntry().level;
		res &= area == testSink->LastEntry().area;
		res &= message == testSink->LastEntry().message;
		return res;
	}
};

TEST_F(LoggerTest, testLOG)
{
	TestSink* testSink = new TestSink;
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(testSink))));

	Timestamp before = std::time(nullptr);
	LOG(LogLevel::ERROR, area) << message;
	Timestamp after = std::time(nullptr);

	ASSERT_EQ(1, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	ASSERT_EQ(area, testSink->LastEntry().area);
	ASSERT_EQ(message, testSink->LastEntry().message);

	before = std::time(nullptr);
	LOG(LogLevel::ERROR) << message;
	after = std::time(nullptr);

	ASSERT_EQ(2, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	string empty;
	ASSERT_EQ(empty, testSink->LastEntry().area);
	ASSERT_EQ(message, testSink->LastEntry().message);
}

TEST_F(LoggerTest, testSLOG)
{
	TestSink* testSink = new TestSink;
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(testSink))));

	Timestamp before = std::time(nullptr);
	SLOG(LogLevel::ERROR, area, message);
	Timestamp after = std::time(nullptr);

	ASSERT_EQ(1, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	ASSERT_EQ(area, testSink->LastEntry().area);
	ASSERT_EQ(message, testSink->LastEntry().message);

	before = std::time(nullptr);
	SLOG(LogLevel::ERROR, message);
	after = std::time(nullptr);

	ASSERT_EQ(2, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	string empty;
	ASSERT_EQ(empty, testSink->LastEntry().area);
	ASSERT_EQ(message, testSink->LastEntry().message);
}

TEST_F(LoggerTest, testSink2)
{
	std::shared_ptr<TestSink> testSink1(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink1)));
	std::shared_ptr<TestSink> testSink2(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink2)));
	Logger::Instance().SetReportingLevel(LogLevel::ERROR);

	ASSERT_TRUE(LogAndCheck(LogLevel::ERROR, area, message, testSink1.get()));
	ASSERT_TRUE(LogAndCheck(LogLevel::ERROR, area, message, testSink2.get()));
}

TEST_F(LoggerTest, testReportingLevel)
{
	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(area, LogLevel::WARNING);
	std::shared_ptr<TestSink> testSink(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink, filter)));
	Logger::Instance().SetReportingLevel(LogLevel::ERROR);

	// LogLevel is equal ReportingLevel, message logged
	LOG(LogLevel::ERROR, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is less than ReportingLevel, message not logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is greater than ReportingLevel, message logged
	LOG(LogLevel::FATAL, area) << message;
	ASSERT_EQ(2, testSink->Count());
}

TEST_F(LoggerTest, testFilter)
{
	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(area, LogLevel::WARNING);
	filter->SetReportingLevel(LogLevel::ERROR);

	std::shared_ptr<TestSink> testSink(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink, filter)));
	Logger::Instance().SetReportingLevel(LogLevel::TRACE);

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	ASSERT_EQ(1, testSink->Count());

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	ASSERT_EQ(2, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	ASSERT_EQ(2, testSink->Count());

	Logger::Instance().SetReportingLevel(LogLevel::ERROR);
	// LogLevel is less than FilterReportingLevel, message not logged
	LOG(LogLevel::WARNING) << message;
	ASSERT_EQ(2, testSink->Count());

	// LogLevel is equal to FilterReportingLevel, message logged
	LOG(LogLevel::ERROR) << message;
	ASSERT_EQ(3, testSink->Count());

	Logger::Instance().SetReportingLevel(LogLevel::NONE);
	// Set global ReportingLevel to NONE nothing is logged
	LOG(LogLevel::FATAL) << message;
	ASSERT_EQ(3, testSink->Count());
}

TEST_F(LoggerTest, testSinkFile)
{
	const char* fileName = "test.log";
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkCout))));
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkFile(fileName)))));
	LOG(LogLevel::ERROR, area) << message;
}

TEST_F(LoggerTest, testAsyncSinkFile)
{
	const char* fileName = "test1.log";
	Logger::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new AsyncSink(SinkPtr(new SinkFile(fileName)))))));

	for (size_t i = 0; i < 20; i++) {
		LOG(LogLevel::ERROR, area) << i;
	}
}

void DoLog()
{
	const size_t NUM_ITER = 100;
	auto id = std::this_thread::get_id();
	for (size_t i = 0; i < NUM_ITER; i++) {
		LOG(LogLevel::INFO, "THREAD_TEST") << "thread_id=" << std::setw(6) << id << " iteration=" << i;
	}
}

TEST_F(LoggerTest, testAsyncSinkFileMultithreaded)
{
	const size_t NUM_THREADS = 8;
	const char* fileName = "test2.log";

	Logger::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new AsyncSink(SinkPtr(new SinkFile(fileName)))))));

	std::vector<std::thread> threads;
	threads.resize(NUM_THREADS);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(DoLog);
	}

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}
}

