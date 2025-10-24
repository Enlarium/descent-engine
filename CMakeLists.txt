cmake_minimum_required(VERSION 3.24)

project(descent LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Include modules for finding external libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Define directories
set(DESCENT_INCLUDE_DIRS
	${CMAKE_SOURCE_DIR}/include
)

add_compile_definitions(_POSIX_C_SOURCE=200809L)
message(STATUS "Build Flags: ${CMAKE_C_FLAGS}")

add_subdirectory(src)
add_subdirectory(examples)
