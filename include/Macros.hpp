#pragma once

#include <Geode/DefaultInclude.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifdef HJFOD_BETTEREDIT_EXPORTING
        #define BE_DLL __declspec(dllexport)
    #else
        #define BE_DLL __declspec(dllimport)
    #endif
#else
    #define BE_DLL
#endif
