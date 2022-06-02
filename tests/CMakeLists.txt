cmake_minimum_required(VERSION 3.10)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
endif(MSVC)

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