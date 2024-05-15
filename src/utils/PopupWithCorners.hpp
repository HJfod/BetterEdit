#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

enum class Corner {
    None,
    Blue,
    Gold,
    Gray,
    Dark,
    Brown,
};

template <class... Args>
class PopupWithCorners : public Popup<Args...> {
private:
    static const char* getCorner(Corner corner) {
        switch (corner) {
            default:
            case Corner::None:  return nullptr;
            case Corner::Blue:  return "rewardCorner_001.png";
            case Corner::Gold:  return "dailyLevelCorner_001.png";
            case Corner::Gray:  return "dailyLevelCorner-gray.png"_spr;
            case Corner::Dark:  return "dailyLevelCorner-dark.png"_spr;
            case Corner::Brown: return "dailyLevelCorner-brown.png"_spr;
        }
    }

protected:
    void addCorners(Corner top, Corner bottom) {
        if (auto bottomSpr = getCorner(bottom)) {
            auto cornerBL = CCSprite::createWithSpriteFrameName(bottomSpr);
            FLAlertLayer::m_mainLayer->addChildAtPosition(cornerBL, Anchor::BottomLeft, ccp(24.75f, 24.75f));

            auto cornerBR = CCSprite::createWithSpriteFrameName(bottomSpr);
            cornerBR->setFlipX(true);
            FLAlertLayer::m_mainLayer->addChildAtPosition(cornerBR, Anchor::BottomRight, ccp(-24.75f, 24.75f));
        }
        if (auto topSpr = getCorner(top)) {
            auto cornerTL = CCSprite::createWithSpriteFrameName(topSpr);
            cornerTL->setFlipY(true);
            FLAlertLayer::m_mainLayer->addChildAtPosition(cornerTL, Anchor::TopLeft, ccp(24.75f, -24.75f));

            auto cornerTR = CCSprite::createWithSpriteFrameName(topSpr);
            cornerTR->setFlipY(true);
            cornerTR->setFlipX(true);
            FLAlertLayer::m_mainLayer->addChildAtPosition(cornerTR, Anchor::TopRight, ccp(-24.75f, -24.75f));
        }
    }
};
