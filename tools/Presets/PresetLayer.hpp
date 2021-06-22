#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class PresetLayer : public BrownAlertDelegate {
    protected:
        int m_nSelectedIndex = 0;
        cocos2d::CCLabelBMFont* m_pSelectedLabel;
        gd::CCMenuItemSpriteExtra* m_pDeleteButton;
        gd::CCMenuItemSpriteExtra* m_pSettingsButton;

        void setup() override;
        void updateLabel(const char*);
    
        void onPage(cocos2d::CCObject*);
        void onCreate(cocos2d::CCObject*);
        void onRemove(cocos2d::CCObject*);
        void onRename(cocos2d::CCObject*);
        
    public:
        static PresetLayer* create();
};
