cmake_minimum_required (VERSION 3.10)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
else(MSVC)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 14)
endif(MSVC)

set(HEADER_FILES logdata.h loglevel.h logging.h interfaces.h logger.h log_entry.h sinks.h formaters.h filters.h sinksimp.h spinlock.h node.h mpscqueue.h)
set(SOURCE_FILES sinks.cpp formaters.cpp logger.cpp filters.cpp sinksimp.cpp)

# Add source to this project's executable.
add_library (asynclog ${SOURCE_FILES} ${HEADER_FILES} )