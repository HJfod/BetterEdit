cmake_minimum_required(VERSION 3.21.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES
	src/*.cpp
)
add_library(EditorAPI SHARED ${SOURCES})

target_include_directories(EditorAPI 
	PUBLIC "include"
)

if (PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(EditorAPI PRIVATE BE_TEAM_EDITOR_API_EXPORTING)
endif()

setup_geode_mod(EditorAPI)
