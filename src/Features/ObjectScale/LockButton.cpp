#include "LockButton.hpp"

bool LockButton::init(CCSprite* sprite, CCObject* target, SEL_MenuHandler menuHandler, SliderLockType lockType) {
    if (!CCMenuItemSpriteExtra::init(sprite, nullptr, target, menuHandler)) {
        return false;
    }

    this->m_lockType = lockType;
    return true;
}

void LockButton::setLockedStatus(bool locked) {
    CCSprite* sprite = nullptr;
    switch (m_lockType) {
        case SliderLockType::LockAbsolute:
            sprite = LockButton::createLockSprite(
                locked ? "GJ_button_02.png" : "GJ_button_04.png",
                locked ? "GJ_lock_001.png" : "GJ_lock_open_00snap 1.png"
            );
            break;
        case SliderLockType::LockSlider:
            sprite = LockButton::createLockSprite(
                locked ? "GJ_button_01.png" : "GJ_button_02.png",
                locked ? "GJ_lock_001.png" : "GJ_lock_open_001.png"
            );
            break;
    }

    this->setNormalImage(sprite);
}