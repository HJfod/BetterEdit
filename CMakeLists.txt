cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(BetterEdit VERSION 1.0.0)

file(GLOB SOURCES
	src/features/*.cpp
	src/features/scaling/*.cpp
	src/features/ZoomLevelText/*.cpp
	src/features/about/*.cpp
	src/editor/*.cpp
	src/utils/*.cpp
	src/scripting/lua/*.c
	src/scripting/exposed/*.cpp
	src/scripting/*.cpp
	src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})


if(WIN32)
	file(GLOB SCRIPTING_SOURCES
		src/scripting/lua/*.c
		src/scripting/exposed/*.cpp
		src/scripting/*.cpp
		src/*.cpp
	)
	target_sources(${PROJECT_NAME} PRIVATE ${SCRIPTING_SOURCES})
	target_compile_definitions(${PROJECT_NAME} PRIVATE SOL_NO_EXCEPTIONS)
	target_include_directories(${PROJECT_NAME} PUBLIC "src" "src/scripting/lua")
	target_precompile_headers(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/src/scripting/lua/sol.hpp>)
endif()


if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pro/)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pro)
endif()

# Bad code will NOT be deployed!
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall -Wextra -Wpedantic
		# $override, $modify, etc. should be allowed
		-Wno-dollar-in-identifier-extension
		# Class init functions all overload a virtual
		-Wno-overloaded-virtual
	)
endif()