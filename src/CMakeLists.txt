cmake_minimum_required (VERSION 3.8)

set(HEADER_FILES logging.h interfaces.h sinks.h formaters.h filters.h log_settings.h log_entry.h sinksimp.h)
set(SOURCE_FILES sinks.cpp formaters.cpp filters.cpp sinksimp.cpp)

# Add source to this project's executable.
add_library (simpleLog ${SOURCE_FILES} ${HEADER_FILES} )