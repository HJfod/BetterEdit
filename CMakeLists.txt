cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(BetterEdit VERSION 1.0.0)

if (NOT DONT_INCLUDE_PRO AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pro/)
	message(STATUS "Including Pro features")
	set(BE_PRO_FOUND TRUE)
else()
	message(STATUS "Including only free features, no Pro")
endif()

file(GLOB SOURCES
	src/features/*.cpp
	src/features/scaling/*.cpp
	src/features/ZoomLevelText/*.cpp
	src/features/about/*.cpp
	src/features/backups/*.cpp
	src/features/ViewTab/*.cpp
	src/editor/*.cpp
	src/utils/*.cpp
	src/*.cpp
)

if (${BE_PRO_FOUND})
	include(pro/ProSources.cmake)
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${PRO_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "src")
if (${BE_PRO_FOUND})
	target_include_directories(${PROJECT_NAME} PUBLIC "")
	target_compile_definitions(${PROJECT_NAME} PUBLIC BETTEREDIT_PRO)

	include(pro/OpenSSL.cmake)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC "BETTEREDIT_VERSION_STR=${MOD_VERSION}")

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
