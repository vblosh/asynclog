cmake_minimum_required (VERSION 3.10)

set(HEADER_FILES logdata.h loglevel.h logging.h interfaces.h logger.h log_entry.h sinks.h formaters.h filters.h sinksimp.h spinlock.h node.h mpscqueue.h)
set(SOURCE_FILES sinks.cpp formaters.cpp logger.cpp filters.cpp sinksimp.cpp)

# Add source to this project's executable.
add_library (asynclog ${SOURCE_FILES} ${HEADER_FILES} )