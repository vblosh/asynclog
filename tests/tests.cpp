// testNullstream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//
#include <iostream>
#include <fstream>
#include <chrono>
#include <gtest/gtest.h>

#include "logging.h"
#include "sinksimp.h"

using namespace simplelogger;

//TEST(simpleLogger, test1)
//{
//	LogSettings::Instance().AddSink(SinkPtr(new SinkFile("test.log")));
//	LogSettings::Instance().AddSink(SinkPtr(new SinkCout));
//	LOG(LogLevel::ERROR) << "Hello, world!";
//}


int main() {

	std::string AREA1("AREA1");

	std::shared_ptr<AreaFilter> filter(new AreaFilter);
	filter->SetFilter(AREA1, LogLevel::WARNING);

	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkFile("test.log")))));
	LogSettings::Instance().AddSink(FilteredSinkPtr(new FilteredSink(SinkPtr(new SinkCout))));
	LogSettings::Instance().SetReportingLevel(LogLevel::TRACE);

	LOG(LogLevel::INFO) << "Hello, world!\n";
	LOG(LogLevel::INFO, AREA1) << "Hello, world!\n";
	LOG(LogLevel::ERROR, AREA1) << "Error!\n";

	//bool perfomaceTest = false;
	//if (perfomaceTest) {


	//	const size_t NUM = 100000;

	//	auto myfile = std::fstream("file.log", std::ios::out);
	//	LogSettings::Instance().SetAppender(&myfile);

	//	auto startTime = std::chrono::high_resolution_clock::now();
	//	for (size_t i = 0; i < NUM; i++)
	//	{
	//		LOG(LogLevel::WARNING, AREA1) << "Iteratation " << i << " of " << NUM << std::endl;
	//	}
	//	auto endTime = std::chrono::high_resolution_clock::now();
	//	myfile.close();

	//	std::cout << "Duration of " << NUM << " logs = " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms\n";
	//}


}

