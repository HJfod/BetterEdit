#pragma once

#include "Warn.hpp"

#ifdef BETTEREDIT_PRO
#include <pro/Pro.hpp>
#include <pro/ui/UI.hpp>
#endif

BE_ALLOW_START
BE_ALLOW_UNUSED_FUNCTION

static bool isProUIEnabled() {
#ifdef BETTEREDIT_PRO
    return pro::isNewUIEnabled();
#else
    return false;
#endif
}
static bool isProEnabled() {
#ifdef BETTEREDIT_PRO
    return pro::isProEnabled();
#else
    return false;
#endif
}

BE_ALLOW_END
