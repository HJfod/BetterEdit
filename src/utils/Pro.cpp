#include "Pro.hpp"
#include <Geode/loader/Loader.hpp>

using namespace geode::prelude;

bool isProUIEnabled() {
    auto pro = Loader::get()->getLoadedMod("hjfod.betteredit-pro");
    return pro && pro->template getSettingValue<bool>("pro-ui");
}
