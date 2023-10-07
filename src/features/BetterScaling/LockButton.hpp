#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

enum SliderLockType {
    LockAbsolute,
    LockSlider,
};

class LockButton : public CCMenuItemSpriteExtra {
public:
    SliderLockType m_lockType;

    bool init(CCSprite* sprite, CCObject* target, SEL_MenuHandler menuHandler, SliderLockType lockType);

    void setLockedStatus(bool locked);

    static LockButton* create(CCSprite* sprite, CCObject* target, SEL_MenuHandler menuHandler, SliderLockType lockType) {
        auto ret = new LockButton;
        if (ret && ret->init(sprite, target, menuHandler, lockType)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static CCSprite* createLockSprite(const char* bg, const char* sprite) {
        auto lockSpr = cocos2d::CCSprite::create(bg);

        auto lockSpr_lock = cocos2d::CCSprite::createWithSpriteFrameName(sprite);
        lockSpr_lock->setPosition(lockSpr->getContentSize() / 2);

        lockSpr->addChild(lockSpr_lock);
        lockSpr->setScale(.45f);
        lockSpr->setContentSize(lockSpr->getScaledContentSize());

        return lockSpr;
    }
};
