#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>

#include "sinksimp.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include "logging.h"

using namespace std;
using namespace asynclog;

string AREA = "PERFOMANCE_TEST";
string COUT = "COUT";
string FILTERED = "FILTERED";

const size_t NUM_ITER = 1000;
const size_t NUM_THREADS = 10;
const size_t FILTERED_RATIO = 10;

uint64_t mean(const std::vector<uint64_t>& v)
{
	uint64_t total = std::accumulate(v.begin(), v.end(), uint64_t(0)); // '0' is the initial value
	return total / v.size();
}

void DoLog()
{
	std::vector<uint64_t> logtimes(NUM_ITER);
	std::vector<uint64_t> filteredtimes(NUM_ITER*FILTERED_RATIO);
	char buf[256];

	auto id = std::this_thread::get_id();
	for (size_t i = 0; i < NUM_ITER; i++) {
		sprintf(buf, "thread_id=%5u iteration=%u", id, (unsigned int)i);
		std::string message(buf);
		auto start_time = std::chrono::high_resolution_clock::now();
		SLOG(LogLevel::INFO, AREA, message);
		//LOG(LogLevel::INFO, AREA) << "thread_id=" << id << " iteration=" << i;
		auto stop_time = std::chrono::high_resolution_clock::now();
		logtimes[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();

		for (size_t j = 0; j < FILTERED_RATIO; j++)
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			LOG(LogLevel::INFO, FILTERED) << "thread_id=" << std::setw(5) << id << " iteration=" << i;
			auto stop_time = std::chrono::high_resolution_clock::now();
			filteredtimes[i * FILTERED_RATIO + j] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
		}
	}

	auto minmax = std::minmax_element(logtimes.begin(), logtimes.end());
	auto meantime = mean(logtimes);

	auto minmaxf = std::minmax_element(filteredtimes.begin(), filteredtimes.end());
	auto meantimef = mean(filteredtimes);
	LOG(LogLevel::INFO, COUT) << "thread_id=" << setw(5) << id << 
		" logged time | min=" << setw(6) << fixed << setprecision(2) << double(*minmax.first)/1000 << " us | "
		"max=" << setw(6) << fixed << setprecision(2) << double(*minmax.second)/1000 << " us | "
		"mean=" << setw(6) << fixed << setprecision(2) << double(meantime)/1000 << " us | " <<
		"not logged time | min= " << setw(6) << fixed << setprecision(2) << double(*minmaxf.first)/1000 << " us | "
		"max = " << setw(6) << fixed << setprecision(2) << double(*minmaxf.second)/1000 << " us | "
		"mean = " << setw(6) << fixed << setprecision(2) << double(meantimef)/1000 << " us | ";

}

void PerfTest()
{
	const char* fileName = "perftest.log";
	
	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(AREA, LogLevel::INFO);
	filter->SetReportingLevel(LogLevel::ERROR);

	Logger::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new AsyncSink(SinkPtr(new SinkFile(fileName)))), filter)));
	
	std::shared_ptr<AreaFilter> filter1(new AreaFilter);
	filter1->SetFilter(COUT, LogLevel::INFO);
	filter1->SetReportingLevel(LogLevel::ERROR);

	Logger::Instance().SetReportingLevel(LogLevel::TRACE);
	
	Logger::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new SinkCout), filter1)));

	LOG(LogLevel::INFO, COUT) << "Perfomanse test started with " << NUM_THREADS << " threads"
		<< " and " << NUM_ITER << " number of iteration. Filtered ratio is set to " << FILTERED_RATIO;

	std::vector<std::thread> threads;
	threads.resize(NUM_THREADS);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(DoLog);
	}

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}

	Logger::Instance().Shutdown();
}

void DoLog1()
{
	std::vector<uint64_t> logtimes(NUM_ITER);
	std::vector<uint64_t> filteredtimes(NUM_ITER * FILTERED_RATIO);

	log4cplus::Logger logger = log4cplus::Logger::getInstance(AREA);
	log4cplus::Logger rootLogger = log4cplus::Logger::getInstance("rootLogger");
	log4cplus::Logger loggerCOUT = log4cplus::Logger::getInstance(COUT);

	auto id = std::this_thread::get_id();
	for (size_t i = 0; i < NUM_ITER; i++) {
		auto start_time = std::chrono::high_resolution_clock::now();
		LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("thread_id=") << std::setw(5) << id << LOG4CPLUS_TEXT(" iteration=") << i);
		auto stop_time = std::chrono::high_resolution_clock::now();
		logtimes[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();

		for (size_t j = 0; j < FILTERED_RATIO; j++)
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			LOG4CPLUS_INFO(rootLogger, LOG4CPLUS_TEXT("thread_id=") << std::setw(5) << id << LOG4CPLUS_TEXT(" iteration=") << i);
			auto stop_time = std::chrono::high_resolution_clock::now();
			filteredtimes[i * FILTERED_RATIO + j] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
		}
	}

	auto minmax = std::minmax_element(logtimes.begin(), logtimes.end());
	auto meantime = mean(logtimes);
	LOG4CPLUS_INFO(loggerCOUT, LOG4CPLUS_TEXT("thread_id=") << std::setw(5) << id << LOG4CPLUS_TEXT(" logging time: min time=") << double(*minmax.first) / 1000
		<< LOG4CPLUS_TEXT(" us, max time=") << double(*minmax.second) / 1000 << LOG4CPLUS_TEXT(" us, mean time=") << double(meantime) / 1000 << LOG4CPLUS_TEXT(" us"));

	//auto minmaxf = std::minmax_element(filteredtimes.begin(), filteredtimes.end());
	//auto meantimef = mean(filteredtimes);
	//LOG4CPLUS_INFO(loggerCOUT, LOG4CPLUS_TEXT("thread_id=") << std::setw(5) << id << LOG4CPLUS_TEXT(" logging time: min time=") << *minmaxf.first
	//	<< LOG4CPLUS_TEXT(" ns, max time=") << *minmaxf.second << LOG4CPLUS_TEXT(" ns, mean time=") << meantimef << LOG4CPLUS_TEXT(" ns"));
}

void PerfTest1()
{
	std::string configFile = "log4cplus.cfg";
	log4cplus::PropertyConfigurator config(configFile);
	config.configure();

	log4cplus::Logger logger = log4cplus::Logger::getInstance(COUT);

	LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Perfomanse test started with ") << NUM_THREADS << LOG4CPLUS_TEXT(" threads")
		<< LOG4CPLUS_TEXT(" and ") << NUM_ITER << LOG4CPLUS_TEXT(" number of iteration. Filtered ratio is set to ") << FILTERED_RATIO);

	std::vector<std::thread> threads;
	threads.resize(NUM_THREADS);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(DoLog1);
	}

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}

	logger.shutdown();
}

int main(int argc, char** argv)
{
	PerfTest();
	//PerfTest1();

	return 0;
}
