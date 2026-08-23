#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <gtest/gtest.h>

#include "logging.h"
#include "sinksimp.h"
#include "testsink.h"
#include "log_areas.h"

using namespace std;
using namespace asynclog;

class LoggerTest : public ::testing::Test {
protected:
	int area = areas::TEST;
	string message = "Hello, world!";

	void SetUp() override {
		Logger::Instance().SetReportingLevel(LogLevel::TRACE);
	}

	void TearDown() override {
		Logger::Instance().Shutdown();
	}

	bool LogAndCheck(LogLevel level, int areaId, const std::string& message, TestSink* testSink) {
		unsigned int oldCount = testSink->Count();
		Timestamp before = std::time(nullptr);
		LOG(LogLevel::ERROR, areaId) << message;
		Timestamp after = std::time(nullptr);

		bool res = true;
		res &= testSink->Count() == oldCount + 1;
		res &= std::difftime(after, before) >= std::difftime(testSink->LastEntry().timestamp, before);
		res &= LogLevel::ERROR == testSink->LastEntry().level;
		res &= areaId == testSink->LastEntry().areaId;
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
	ASSERT_EQ(area, testSink->LastEntry().areaId);
	ASSERT_EQ(message, testSink->LastEntry().message);

	before = std::time(nullptr);
	LOG(LogLevel::ERROR) << message;
	after = std::time(nullptr);

	ASSERT_EQ(2, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	ASSERT_EQ(0, testSink->LastEntry().areaId);
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
	ASSERT_EQ(area, testSink->LastEntry().areaId);
	ASSERT_EQ(message, testSink->LastEntry().message);

	before = std::time(nullptr);
	SLOG(LogLevel::ERROR, message);
	after = std::time(nullptr);

	ASSERT_EQ(2, testSink->Count());
	ASSERT_GE(std::difftime(after, before), std::difftime(testSink->LastEntry().timestamp, before));
	ASSERT_EQ(LogLevel::ERROR, testSink->LastEntry().level);
	ASSERT_EQ(0, testSink->LastEntry().areaId);
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
	std::shared_ptr<TestSink> testSink(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink, filter)));
	Logger::Instance().SetReportingLevel(LogLevel::ERROR);

	// LogLevel is equal ReportingLevel, message logged
	LOG(LogLevel::ERROR, area) << message;
	EXPECT_EQ(1, testSink->Count());

	// LogLevel is less than ReportingLevel, message not logged
	LOG(LogLevel::WARNING, area) << message;
	EXPECT_EQ(1, testSink->Count());

	// LogLevel is greater than ReportingLevel, message logged
	LOG(LogLevel::FATAL, area) << message;
	EXPECT_EQ(2, testSink->Count());
}

TEST_F(LoggerTest, testFilter)
{
	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(area, LogLevel::WARNING);

	std::shared_ptr<TestSink> testSink(new TestSink);
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(testSink, filter)));
	Logger::Instance().SetReportingLevel(LogLevel::TRACE);

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	EXPECT_EQ(1, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	EXPECT_EQ(1, testSink->Count());

	// LogLevel is equal than FilterLevel, message logged
	LOG(LogLevel::WARNING, area) << message;
	EXPECT_EQ(2, testSink->Count());

	// LogLevel is less than FilterLevel, message not logged
	LOG(LogLevel::INFO, area) << message;
	EXPECT_EQ(2, testSink->Count());

	Logger::Instance().SetReportingLevel(LogLevel::ERROR);
	// LogLevel is less global ReportingLevel, message not logged
	LOG(LogLevel::WARNING) << message;
	EXPECT_EQ(2, testSink->Count());

	// LogLevel is equal to global ReportingLevel, message logged
	LOG(LogLevel::ERROR) << message;
	EXPECT_EQ(3, testSink->Count());

	Logger::Instance().SetReportingLevel(LogLevel::NONE);
	// Set global ReportingLevel to NONE nothing is logged
	LOG(LogLevel::FATAL) << message;
	EXPECT_EQ(3, testSink->Count());

	// global ReportingLevel is NONE but 
	// LogLevel is less than FilterLevel, message is logged
	LOG(LogLevel::WARNING, area) << message;
	EXPECT_EQ(4, testSink->Count());
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
			SinkPtr(new AsyncSink(SinkPtr(new SinkFile(fileName)), 2)))));

	for (size_t i = 0; i < 100; i++) {
		LOG(LogLevel::ERROR, area) << i;
	}
}

void DoLog()
{
	const size_t NUM_ITER = 100;
	auto id = std::this_thread::get_id();
	for (size_t i = 0; i < NUM_ITER; i++) {
		LOG(LogLevel::INFO, areas::TEST) << "thread_id=" << std::setw(6) << id << " iteration=" << i;
	}
}

TEST_F(LoggerTest, testAsyncSinkFileMultithreaded)
{
	Logger::Instance().SetReportingLevel(LogLevel::TRACE);

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

TEST_F(LoggerTest, testJsonFormatter)
{
	EXPECT_EQ("INFO", getLogLevelName(LogLevel::INFO));
	EXPECT_EQ("ERROR", getLogLevelName(LogLevel::ERROR));
	EXPECT_EQ("INFO", LogLevelLabels[static_cast<size_t>(LogLevel::INFO)]);

	JsonFormatter formatter("%Y-%m-%d %H:%M:%S");
	Logdata data(LogLevel::INFO, areas::NETWORK, "Connection from \"127.0.0.1\"\nstatus: OK");

	std::ostringstream ss;
	formatter.Format(ss, data);

	std::string output = ss.str();
	EXPECT_NE(output.find("\"level\":\"INFO\""), std::string::npos);
	EXPECT_NE(output.find("\"area\":\"NETWORK\""), std::string::npos);
	EXPECT_NE(output.find("\"message\":\"Connection from \\\"127.0.0.1\\\"\\nstatus: OK\""), std::string::npos);
	EXPECT_EQ(output.front(), '{');
	EXPECT_EQ(output[output.size() - 2], '}'); // before newline
}

TEST_F(LoggerTest, testCompositeSink)
{
	std::shared_ptr<TestSink> testSink1 = std::make_shared<TestSink>();
	std::shared_ptr<TestSink> testSink2 = std::make_shared<TestSink>();

	auto composite = std::make_shared<CompositeSink>(std::initializer_list<SinkPtr>{testSink1, testSink2});
	Logger::Instance().AddSink(FilteredSinkPtr(new FilteredSink(composite)));

	LOG(LogLevel::ERROR, area) << "Composite test message";

	EXPECT_EQ(1, testSink1->Count());
	EXPECT_EQ("Composite test message", testSink1->LastEntry().message);
	EXPECT_EQ(area, testSink1->LastEntry().areaId);
	EXPECT_EQ(LogLevel::ERROR, testSink1->LastEntry().level);

	EXPECT_EQ(1, testSink2->Count());
	EXPECT_EQ("Composite test message", testSink2->LastEntry().message);
	EXPECT_EQ(area, testSink2->LastEntry().areaId);
	EXPECT_EQ(LogLevel::ERROR, testSink2->LastEntry().level);
}

TEST_F(LoggerTest, testCompositeSinkWithAsyncSink)
{
	std::shared_ptr<TestSink> testSink1 = std::make_shared<TestSink>();
	std::shared_ptr<TestSink> testSink2 = std::make_shared<TestSink>();

	auto composite = std::make_shared<CompositeSink>();
	composite->AddSink(testSink1);
	composite->AddSink(testSink2);

	Logger::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new AsyncSink(composite)))));

	for (size_t i = 0; i < 20; ++i) {
		LOG(LogLevel::ERROR, area) << "Async composite " << i;
	}

	Logger::Instance().Shutdown();

	EXPECT_EQ(20, testSink1->Count());
	EXPECT_EQ(20, testSink2->Count());
	EXPECT_EQ("Async composite 19", testSink1->LastEntry().message);
	EXPECT_EQ("Async composite 19", testSink2->LastEntry().message);
}


