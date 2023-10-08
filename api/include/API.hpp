#pragma once

#include <Geode/DefaultInclude.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifdef BE_TEAM_EDITOR_API_EXPORTING
        #define EDITOR_API_EXPORT __declspec(dllexport)
    #else
        #define EDITOR_API_EXPORT __declspec(dllimport)
    #endif
#else
    #define EDITOR_API_EXPORT
#endif
