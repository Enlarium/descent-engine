cmake_minimum_required(VERSION 3.24)

project(descent LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Options
option(DESCENT_BUILD_TESTS "Build Descent test programs" OFF)
option(DESCENT_BUILD_EXAMPLES "Build Descent example programs" OFF)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Include modules for finding external libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find necessary packages
find_package(Lua REQUIRED)
find_package(Vulkan REQUIRED)

# Define directories
set(DESCENT_INCLUDE_DIRS
	${CMAKE_SOURCE_DIR}/include
)

# Global compile definitions
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_definitions(_GNU_SOURCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_definitions(_DARWIN_C_SOURCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	add_compile_definitions(_POSIX_C_SOURCE=200809L)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_compile_definitions(_WIN64)
    else()
        add_compile_definitions(_WIN32)
    endif()
endif()
endif()

# Subdirectories
add_subdirectory(src)

# Examples
if(DESCENT_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# Tests
if(DESCENT_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

