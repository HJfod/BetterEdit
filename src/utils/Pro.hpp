#pragma once

#include "Warn.hpp"

#ifdef BETTEREDIT_PRO
#include <pro/Pro.hpp>
#include <pro/ui/UI.hpp>
#endif

BE_ALLOW_START
BE_ALLOW_UNUSED_FUNCTION

#ifdef BETTEREDIT_PRO
    #define BE_PRO_FEATURE(...) \
        pro::checkProEnabled() \
            .listen([this](auto* value) { \
                if (pro::verifyPro<__LINE__>(*value) == (__LINE__ + pro::ssl::MAGIC_OK)) { \
                    __VA_ARGS__; \
                } \
            })

    #define BE_PRO_FEATURE_GLOBAL(...) \
        pro::checkProEnabled() \
            .listen([](auto* value) { \
                if (pro::verifyPro<__LINE__>(*value) == (__LINE__ + pro::ssl::MAGIC_OK)) { \
                    __VA_ARGS__; \
                } \
            })
#else
    #define BE_PRO_FEATURE(...)
    #define BE_PRO_FEATURE_GLOBAL(...)
#endif

BE_ALLOW_END
