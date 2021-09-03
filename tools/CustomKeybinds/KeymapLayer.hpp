#pragma once

#include "KeybindManager.hpp"
#include "KeybindingsLayer.hpp"
#include <BrownAlertDelegate.hpp>

class KeymapLayer : public BrownAlertDelegate, public FLAlertLayerProtocol {
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
        std::set<enumKeyCodes> m_ePressed;
        std::string m_sLabelString = "";
        ccColor3B m_colNormal = { 255, 255, 255 };
        ccColor3B m_colToggleable = m_colNormal;
        ccColor3B m_colToggled = m_colNormal;
        ccColor3B m_colInUseEditor = { 0, 255, 50 };
        ccColor3B m_colInUsePlay = { 255, 255, 80  };
        ccColor3B m_colInUseGlobal = { 205, 50, 255 };
        ccColor3B m_colInUse = { 50, 90, 255 };
        ccColor3B m_colConflict = { 255, 80, 20 };
        ccColor3B m_colText = { 255, 255, 255 };
        ButtonSprite* m_pSelected;
        std::vector<CCMenuItemSpriteExtra*> m_pBindBtns;
        KeybindingsLayer_CB* m_pKeybindingsLayer;

        void FLAlert_Clicked(FLAlertLayer*, bool);

        void setup() override;
        CCMenuItemSpriteExtra* getKeyButton(const char*, int, int, CCPoint const&, bool = false);
        CCMenuItemToggler* getKeyToggle(const char*, int, CCMenuItemToggler**, SEL_MenuHandler, CCPoint const&);
        void updateKey(ButtonSprite*);

        void keyDown(enumKeyCodes) override;
        void keyUp(enumKeyCodes) override;
        void visit() override;

        Keybind getKeybind(enumKeyCodes = KEY_None);

        void onToggle(CCObject*);
        void onSelect(CCObject*);
        void onSetBind(CCObject*);
        void onUnbind(CCObject*);

        ~KeymapLayer();

    public:
        void updateKeys();
        void showHoveredKey();
        void loadKeybind(Keybind const&);
    
        static KeymapLayer* create(KeybindingsLayer_CB*);
};
