cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BetterEdit VERSION 4.6.0)

file(GLOB SOURCES
	src/features/backups/*.cpp
	src/features/autolink/*.cpp
	src/features/*.cpp
	src/tracking/*.cpp
	src/script/*.cpp
	src/other/*.cpp
	src/ui/*.cpp
	src/*.cpp
)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC
    src
)

option(CLIPPER2_UTILS "Build utilities" Off)
option(CLIPPER2_EXAMPLES "Build examples" Off)
option(CLIPPER2_TESTS "Build tests" Off)
add_subdirectory(src/external/Clipper2)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

set(GEODE_LINK_NIGHTLY On)
add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

target_link_libraries(${PROJECT_NAME} Clipper2)
setup_geode_mod(${PROJECT_NAME})
