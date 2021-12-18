#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>

#include "logging.h"
#include "sinksimp.h"

using namespace std;
using namespace asynclog;

string AREA = "PERFOMANCE_TEST";
const size_t NUM_ITER = 100;
const size_t NUM_THREADS = 10;
const size_t BUF_LEN = 1024;
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

	auto id = std::this_thread::get_id();
	for (size_t i = 0; i < NUM_ITER; i++) {
		auto start_time = std::chrono::high_resolution_clock::now();
		LOG(LogLevel::INFO, AREA) << "thread_id=" << std::setw(5) << id << " iteration=" << i;
		auto stop_time = std::chrono::high_resolution_clock::now();
		logtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();

		for (size_t j = 0; j < FILTERED_RATIO; j++)
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			LOG(LogLevel::INFO) << "thread_id=" << std::setw(5) << id << " iteration=" << i;
			auto stop_time = std::chrono::high_resolution_clock::now();
			filteredtimes[i * FILTERED_RATIO + j] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
		}
	}

	auto minmax = std::minmax_element(logtimes.begin(), logtimes.end());
	auto meantime = mean(logtimes);
	LOG(LogLevel::INFO, AREA) << "thread_id=" << std::setw(5) << id << " logging time: min time=" << *minmax.first
		<< " us, max time=" << *minmax.second << " us, mean time=" << meantime << " us";

	auto minmaxf = std::minmax_element(filteredtimes.begin(), filteredtimes.end());
	auto meantimef = mean(filteredtimes);
	LOG(LogLevel::INFO, AREA) << "thread_id=" << std::setw(5) << id << " logging time: min time=" << *minmaxf.first
		<< " ns, max time=" << *minmaxf.second << " ns, mean time=" << meantimef << " ns";

}

void PerfTest()
{
	char* fileName = "perftest.log";
	
	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(AREA, LogLevel::INFO);

	LogSettings::Instance().AddSink(
		FilteredSinkPtr(new FilteredSink(
			SinkPtr(new AsyncSink(BUF_LEN,
				SinkPtr(new SinkFile(fileName)))), filter)));
	LogSettings::Instance().SetReportingLevel(LogLevel::ERROR);

	LOG(LogLevel::INFO, AREA) << "Perfomanse test started with " << NUM_THREADS << " threads"
		<< " and " << NUM_ITER << " number of iteration. Filtered ratio is set to " << FILTERED_RATIO
		<< " and initial buffer size is " << BUF_LEN;

	std::vector<std::thread> threads;
	threads.resize(NUM_THREADS);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(DoLog);
	}

	for (size_t i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}

	LogSettings::Instance().Shutdown();
}

int main(int argc, char** argv)
{
	PerfTest();

	return 0;
}