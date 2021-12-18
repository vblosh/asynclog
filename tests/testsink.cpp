// testNullstream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "testsink.h"

using namespace asynclog;

void TestSink::Log(const Logdata& logdata)
{
	lastEntry = logdata;
	++count;
}
