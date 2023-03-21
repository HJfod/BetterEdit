#pragma once

#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class AutoLinkClass : public CCObject {};

class AutoLinkManager {
protected:
    std::vector<Ref<AutoLinkClass>> m_autoLinks;
};

