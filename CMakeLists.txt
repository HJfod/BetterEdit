cmake_minimum_required(VERSION 3.21.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BetterEdit VERSION 4.6.0)

file(GLOB SOURCES
	src/Features/RotateSaws/*.cpp
	src/Features/ZoomText/*.cpp
	src/Features/ViewTab/*.cpp
	src/Features/MoreTabs/*.cpp
	src/Features/VisibilityToggle/*.cpp
	src/Other/*.cpp
	src/Features/*.cpp
	src/*.cpp
)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC
    src
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

setup_geode_mod(${PROJECT_NAME})
