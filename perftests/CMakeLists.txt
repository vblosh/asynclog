cmake_minimum_required(VERSION 3.10)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
else(MSVC)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 14)
endif(MSVC)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(LOG_INCLUDE "../src/")
set(HEADER_FILES )
set(SOURCE_FILES tests.cpp)
add_executable(perftests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(perftests PRIVATE ${LOG_INCLUDE})
target_link_libraries(perftests PRIVATE asynclog Threads::Threads)
