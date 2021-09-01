#pragma once

#include "KeybindManager.hpp"
#include <BrownAlertDelegate.hpp>

class KeymapLayer : public BrownAlertDelegate {
    protected:
        CCArray* m_pKeyBtns;

        void setup() override;
        CCMenuItemSpriteExtra* getKeyButton(const char*, int, CCPoint const&);
        CCMenuItemToggler* getKeyToggle(const char*, int, SEL_MenuHandler, CCPoint const&);

        void keyDown(enumKeyCodes) override;
        void keyUp(enumKeyCodes) override;

        ~KeymapLayer();

    public:
        static KeymapLayer* create();
};
