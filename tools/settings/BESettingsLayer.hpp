#pragma once

#include <GDMake.h>
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>
#include "../../BetterEdit.hpp"

class MoreOptionsLayer : public gd::FLAlertLayer,
    gd::TextInputDelegate,
    gd::GooglePlayDelegate
{
    protected:
        bool init() {
            return reinterpret_cast<bool(__fastcall*)(MoreOptionsLayer*)>(
                gd::base + 0x1de8f0
            )(this);
        };
};

class BESettingsLayer : public BrownAlertDelegate {
    protected:
        int m_nToggleCount;
        gd::CCMenuItemSpriteExtra* m_pPrevPageBtn;
        gd::CCMenuItemSpriteExtra* m_pNextPageBtn;

        void setup() override;
        void addToggle(const char* text, const char* desc, const char* key);
        void onToggle(cocos2d::CCObject*);

    public:
        static BESettingsLayer* create();
};
