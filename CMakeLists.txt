# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("asynclog" VERSION 0.1)

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory ("tests")
add_subdirectory ("perftests")
add_subdirectory ("perfcompare")

