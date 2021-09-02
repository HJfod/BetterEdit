#pragma once

#include "KeybindManager.hpp"
#include <BrownAlertDelegate.hpp>

class KeymapLayer : public BrownAlertDelegate {
    protected:
        static constexpr int s_fItemWidth = 12;
        static constexpr int s_fItemSeparation = 10;
        CCArray* m_pKeyBtns;
        bool m_bControlDown = false;
        bool m_bShiftDown = false;
        bool m_bAltDown = false;
        bool m_bUpdateOnNextVisit = false;
        CCMenuItemToggler* m_pControlToggle;
        CCMenuItemToggler* m_pShiftToggle;
        CCMenuItemToggler* m_pAltToggle;
        TextArea* m_pCallbackLabel;
        Keybind m_obHovered;
        std::string m_sLabelString = "";

        void setup() override;
        CCMenuItemSpriteExtra* getKeyButton(const char*, int, int, CCPoint const&);
        CCMenuItemToggler* getKeyToggle(const char*, int, CCMenuItemToggler**, SEL_MenuHandler, CCPoint const&);

        void keyDown(enumKeyCodes) override;
        void keyUp(enumKeyCodes) override;
        void visit() override;

        Keybind getKeybind(enumKeyCodes);

        void onToggle(CCObject*);

        ~KeymapLayer();

    public:
        void updateKeys();
        void showHoveredKey();
    
        static KeymapLayer* create();
};
