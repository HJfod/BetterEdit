#include "Pro.hpp"

#ifdef BETTEREDIT_PRO
bool be::isProUIEnabled() {
    return pro::isNewUIEnabled();
}
#else
bool be::isProUIEnabled() {
    return false;
}
#endif
