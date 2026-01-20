cmake_minimum_required(VERSION 3.23)

project(descent LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Unset global C flags - we use per-target flags
set(CMAKE_C_FLAGS "")

# Options
option(DESCENT_BUILD_ARM      "Build for ARM instead of x86"             OFF)
option(DESCENT_BUILD_32       "Build for 32 bit instead of 64 bit"       OFF)
option(DESCENT_BUILD_TESTS    "Build Descent test programs"              OFF)
option(DESCENT_BUILD_EXAMPLES "Build Descent example programs"           OFF)
option(DESCENT_IWYU           "Run include-what-you-use while compiling" OFF)

# Require out-of-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source build not allowed!")
endif()

# Find packages
find_package(Vulkan REQUIRED)

# Find programs
if(DESCENT_IWYU)
find_program(IWYU_PATH NAMES include-what-you-use iwyu)
if(NOT IWYU_PATH)
	message(FATAL_ERROR "include-what-you-use not found!")
endif()
endif()

# Include modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(target_enable_iwyu)

# Define directories
set(DESCENT_INCLUDE_DIRS
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/intern
	${CMAKE_SOURCE_DIR}/extern/lua/include
	${CMAKE_SOURCE_DIR}/extern/tomlc17/include
	${CMAKE_SOURCE_DIR}/extern/yyjson/include
	)

# Get target architecture
if(DESCENT_BUILD_ARM)
	if(DESCENT_BUILD_32)
		set(MARCH_FLAGS "-march=armv7-a -m32")
	else()
		set(MARCH_FLAGS "-march=armv8-a -m64")
	endif()
else()
	if(DESCENT_BUILD_32)
		set(MARCH_FLAGS "-march=i386 -m32")
	else()
		set(MARCH_FLAGS "-march=x86-64 -m64")
	endif()
endif()

# Compilation definitions
separate_arguments(BASE_DEFINITIONS)
set(DESCENT_DEFINITIONS ${DEFINITIONS})

# Compilation flags
separate_arguments(MARCH_FLAGS)
separate_arguments(BASE_FLAGS)
separate_arguments(MODE_FLAGS)
set(DESCENT_FLAGS ${MARCH_FLAGS} ${BASE_FLAGS} ${MODE_FLAGS})

# Configure metadata
set(DESCENT_VERSION_MAJOR "0")
set(DESCENT_VERSION_MINOR "0")
set(DESCENT_VERSION_PATCH "0")
set(DESCENT_VERSION_VARIANT "dev")

if(DESCENT_VERSION_VARIANT)
	set(DESCENT_VERSION "${DESCENT_VERSION_MAJOR}.${DESCENT_VERSION_MINOR}.${DESCENT_VERSION_PATCH}-${DESCENT_VERSION_VARIANT}")
else()
	set(DESCENT_VERSION "${DESCENT_VERSION_MAJOR}.${DESCENT_VERSION_MINOR}.${DESCENT_VERSION_PATCH}")
endif()

message(STATUS "Descent version: ${DESCENT_VERSION}")

configure_file(${CMAKE_SOURCE_DIR}/config/Doxyfile.in   ${CMAKE_SOURCE_DIR}/Doxyfile @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/metadata.h.in ${CMAKE_SOURCE_DIR}/include/descent/metadata.h @ONLY)

# Subdirectories
add_subdirectory(extern)
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
