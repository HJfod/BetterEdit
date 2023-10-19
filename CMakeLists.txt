cmake_minimum_required(VERSION 3.21.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BetterEdit VERSION 5.0.0)

file(GLOB SOURCES
	src/features/RotateSaws/*.cpp
	src/features/ZoomText/*.cpp
	src/features/ViewTab/*.cpp
	src/features/MoreTabs/*.cpp
	src/features/VisibilityToggle/*.cpp
	src/features/BetterScaling/*.cpp
	src/features/StartPosSwitcher/*.cpp
	src/features/OffsetNextFree/*.cpp
	src/features/RGBColorInput/*.cpp
	src/features/*.cpp
	src/other/*.cpp
	src/*.cpp
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC "src")

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

setup_geode_mod(${PROJECT_NAME} EXTERNALS be_team.editorapi:1.0.0)


# Build EditorAPI alongside BetterEdit
add_subdirectory(api)
target_link_libraries(${PROJECT_NAME} EditorAPI)

# For HJfod's pro version
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pro")
	add_subdirectory(pro)
endif()
