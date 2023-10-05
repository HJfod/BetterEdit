#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>

using namespace geode::prelude;

static ButtonSprite* createEditBtnSprite(
    const char* file, bool sel
) {
    auto sprOff = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName(file),
        0x32, 0, 0x32, 1, true, sel ? "GJ_button_02.png" : "GJ_button_01.png", true
    );
    sprOff->m_subSprite->setScale(1.25f);

    return sprOff;
}

class VisibilityToggle : public CCMenuItemToggler {
    public:
        using Getter = std::function<bool(void)>;
        using Setter = std::function<void(bool, VisibilityToggle*)>;

    protected:
        Getter m_pGetter;
        Setter m_pSetter;

        void onToggle(CCObject*);
    
    public:
        ~VisibilityToggle();
        void updateState();
        VisibilityToggle* invokeSetter();

        static VisibilityToggle* create(
            const char* sprite,
            Getter getter,
            Setter setter
        );

        static VisibilityToggle* create(
            const char* sprite
        );
};