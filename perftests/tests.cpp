#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>

#include "sinksimp.h"

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
	uint64_t total = std::accumulate(v.begin(), v.end(), uint64_t(0));
	return total / v.size();
}

tuple<double, double, double> get_min_median_max_us(std::vector<uint64_t>& v)
{
	static constexpr double NS2US = 1000.0;
	std::sort(v.begin(), v.end());

	return make_tuple(double(*v.begin()) / NS2US
		, double(*(v.begin() + v.size() / 2) + *(v.begin() + v.size() / 2 + 1)) / (2.0 * NS2US)
		, double(*(v.end() - 1)) / NS2US);
}

void DoLog()
{
	std::vector<uint64_t> logtimes(NUM_ITER);
	std::vector<uint64_t> filteredtimes(NUM_ITER * FILTERED_RATIO);
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

	double min, median, max;
	std::tie(min, median, max) = get_min_median_max_us(logtimes);

	double minf, medianf, maxf;
	std::tie(minf, medianf, maxf) = get_min_median_max_us(filteredtimes);
	LOG(LogLevel::INFO, COUT) << "thread_id=" << setw(5) << id <<
		" logged time | min=" << setw(6) << fixed << setprecision(2) << min << " us | "
		"median=" << setw(6) << fixed << setprecision(2) << median << " us | " <<
		"max=" << setw(6) << fixed << setprecision(2) << max << " us | " <<
		"not logged time | min= " << setw(6) << fixed << setprecision(2) << minf << " us | "
		"median = " << setw(6) << fixed << setprecision(2) << medianf << " us | " <<
		"max = " << setw(6) << fixed << setprecision(2) << maxf << " us | ";
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

int main(int argc, char** argv)
{
	PerfTest();

	return 0;
}
