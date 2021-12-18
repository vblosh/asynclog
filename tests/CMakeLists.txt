cmake_minimum_required(VERSION 3.10)

project(asynclogTests)

#GTEST
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(LOG_INCLUDE "../src/")
set(HEADER_FILES testsink.h)
set(SOURCE_FILES tests.cpp testsink.cpp)

add_executable(runUnitTests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(runUnitTests PRIVATE ${LOG_INCLUDE})
target_link_libraries(runUnitTests asynclog GTest::GTest GTest::Main)

add_test( runUnitTests runUnitTests )