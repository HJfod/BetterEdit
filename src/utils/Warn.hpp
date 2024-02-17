#pragma once

// https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/

#if defined(_MSC_VER)
    #define BE_ALLOW_START               __pragma(warning( push ))
    #define BE_ALLOW_END                 __pragma(warning( pop )) 
    #define BE_ALLOW_MSVC(warningNumber) __pragma(warning( disable : warningNumber ))

    #define BE_ALLOW_SHADOWING    BE_ALLOW_MSVC(4458)
    #define BE_ALLOW_FAKE_ENUMS   BE_ALLOW_MSVC(4063)
#elif defined(__GNUC__) || defined(__clang__)
    #define DO_PRAGMA(X) _Pragma(#X)
    #define BE_ALLOW_START              DO_PRAGMA(GCC diagnostic push)
    #define BE_ALLOW_END                DO_PRAGMA(GCC diagnostic pop) 
    #define BE_ALLOW_CLANG(warningName) DO_PRAGMA(GCC diagnostic ignored #warningName)
    
    #define BE_ALLOW_SHADOWING    BE_ALLOW_CLANG(-Wshadow)
    #define BE_ALLOW_FAKE_ENUMS
#else
    #define BE_ALLOW_START
    #define BE_ALLOW_END
    #define BE_ALLOW_SHADOWING
    #define BE_ALLOW_FAKE_ENUMS
#endif
