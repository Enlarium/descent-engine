cmake_minimum_required(VERSION 3.24)

project(descent LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Options
option(DESCENT_BUILD_TESTS "Build Descent test programs" OFF)
option(DESCENT_BUILD_EXAMPLES "Build Descent example programs" OFF)
option(DESCENT_IWYU "Run include-what-you-use while compiling" OFF)

# IWYU function
function(target_enable_iwyu target)
	if(DESCENT_IWYU)
		if(IWYU_PATH)
			get_target_property(is_imported ${target} IMPORTED)
			if(NOT is_imported)
				set_property(TARGET ${target} PROPERTY C_INCLUDE_WHAT_YOU_USE "${IWYU_PATH};${IWYU_OPTIONS}")
				set_property(TARGET ${target} PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH};${IWYU_OPTIONS}")
			endif()
		else()
			message(WARNING "include-what-you-use not found")
		endif()
	endif()
endfunction()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include modules for finding external libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find necessary programs
find_program(IWYU_PATH NAMES include-what-you-use iwyu)

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

get_property(all_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
foreach(tgt IN LISTS all_targets)
	target_enable_iwyu(${tgt})
endforeach()