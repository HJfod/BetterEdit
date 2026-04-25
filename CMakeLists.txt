cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(BetterEdit VERSION 1.0.0)

file(GLOB SOURCES
	src/features/*.cpp
	src/features/scaling/*.cpp
	src/features/ZoomLevelText/*.cpp
	src/features/about/*.cpp
	src/features/backups/*.cpp
	src/features/ViewTab/*.cpp
	src/features/scripting/*.cpp
	src/features/supporters/*.cpp
	src/server/*.cpp
	src/editor/*.cpp
	src/utils/*.cpp
	src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode SYSTEM)

setup_geode_mod(${PROJECT_NAME})
# For server stuff we need some lib for RSA and rn we are using wolfSSL because 
# dank_meme recommended it
set(WOLFSSL_ENCKEYS ON)
CPMAddPackage("gh:wolfSSL/wolfssl@5.8.0-stable")
target_link_libraries(${PROJECT_NAME} wolfssl)
target_compile_definitions(wolfssl PUBLIC WOLFSSL_PUB_PEM_TO_DER)
target_compile_definitions(${PROJECT_NAME} PUBLIC WOLFSSL_PUB_PEM_TO_DER)

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
		# GEODE_UNWRAP
		-Wno-gnu-statement-expression-from-macro-expansion
		# Style "IDC"s
		-Wno-extra-semi
		# QuickJS
		-Wno-format-nonliteral
	)
endif()
