cmake_minimum_required(VERSION 3.10)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
endif(MSVC)

include(FindPkgConfig)
pkg_check_modules(LOG4CPLUS REQUIRED log4cplus)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(LOG_INCLUDE "../src/")
set(HEADER_FILES )
set(SOURCE_FILES tests.cpp)
add_executable(perfcompare ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(perfcompare PRIVATE ${LOG_INCLUDE} ${LOG4CPLUS_INCLUDE_DIRS})
target_link_libraries(perfcompare PRIVATE asynclog Threads::Threads ${LOG4CPLUS_LIBRARIES})
