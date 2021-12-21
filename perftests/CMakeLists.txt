cmake_minimum_required(VERSION 3.10)

project(asynclogTests)

find_package(log4cplus CONFIG REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(LOG_INCLUDE "../src/")
set(HEADER_FILES )
set(SOURCE_FILES tests.cpp)
add_executable(perftests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(perftests PRIVATE ${LOG_INCLUDE})
target_link_libraries(perftests PRIVATE asynclog Threads::Threads log4cplus::log4cplus)
