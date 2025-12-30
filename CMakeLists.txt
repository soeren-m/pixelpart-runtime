cmake_minimum_required(VERSION 3.21)

set(PIXELPART_RUNTIME_VERSION 1.8.4)

project(pixelpart-runtime
	VERSION ${PIXELPART_RUNTIME_VERSION}
	LANGUAGES C CXX)

# Options
option(PIXELPART_RUNTIME_MULTITHREADING "Multi-threading support" OFF)
option(PIXELPART_RUNTIME_SIMD "SIMD support" OFF)

# Messages
message(STATUS "Pixelpart runtime: Version ${PIXELPART_RUNTIME_VERSION}")

if(PIXELPART_RUNTIME_MULTITHREADING)
	message(STATUS "Pixelpart runtime: Multi-threading support enabled")
else()
	message(STATUS "Pixelpart runtime: Multi-threading support disabled")
endif()

if(PIXELPART_RUNTIME_SIMD)
	message(STATUS "Pixelpart runtime: SIMD support enabled")
else()
	message(STATUS "Pixelpart runtime: SIMD support disabled")
endif()

# C++ settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pixelpart-runtime STATIC)

# Multi-threading support
if(PIXELPART_RUNTIME_MULTITHREADING)
	find_package(Threads REQUIRED)

	target_compile_definitions(pixelpart-runtime PUBLIC
		PIXELPART_RUNTIME_MULTITHREADING)
	target_link_libraries(pixelpart-runtime PUBLIC
		Threads::Threads)
endif()

# SIMD support
if(PIXELPART_RUNTIME_SIMD)
	target_compile_definitions(pixelpart-runtime PUBLIC
		GLM_FORCE_INTRINSICS)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Subdirectories
add_subdirectory(asset)
add_subdirectory(common)
add_subdirectory(effect)
add_subdirectory(engine)
add_subdirectory(glm)
add_subdirectory(vertex)
add_subdirectory(zlib)
