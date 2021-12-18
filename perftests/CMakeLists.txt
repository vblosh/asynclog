cmake_minimum_required(VERSION 3.10)

project(asynclogTests)

set(LOG_INCLUDE "../src/")
set(HEADER_FILES )
set(SOURCE_FILES tests.cpp)
add_executable(perftests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(perftests PRIVATE ${LOG_INCLUDE})
target_link_libraries(perftests asynclog)
