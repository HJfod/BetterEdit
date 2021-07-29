#pragma once

#include <GDMake.h>
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>
#include "../../BetterEdit.hpp"
#include <array>

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

using BE_Callback = void(*)(std::string const&);
using BE_Callback_B = void(*)(bool);

class BESettingsLayer : public BrownAlertDelegate, gd::TextInputDelegate {
    protected:
        unsigned int m_nItemCount = 0;
        int m_nCurrentPage = 0;
        unsigned int m_nDestPage = 0;
        std::vector<std::vector<cocos2d::CCNode*>> m_vPages;
        gd::CCMenuItemSpriteExtra* m_pPrevPageBtn;
        gd::CCMenuItemSpriteExtra* m_pNextPageBtn;

        static constexpr const float s_fItemPaddingV = 35.0f;
        static constexpr const unsigned int s_nMaxItemsOnPage = 10;

        void setup() override;
        void onClose(cocos2d::CCObject*);
        void keyDown(enumKeyCodes) override;

        cocos2d::CCPoint getItemPos(bool large = false, bool center = false);
        void incrementPageCount(bool skip = false);
        void addItem(cocos2d::CCNode* item);

        void addInput(
            const char* text,
            std::string const& value,
            BE_Callback cb, 
            std::string const& filter = "0123456789"
        );
        void addToggle(const char* text, const char* desc, bool value, BE_Callback_B cb, bool experimental = false);
        void addSlider(const char* text, cocos2d::SEL_MenuHandler onChange, float val = 0.0f);
        cocos2d::CCLabelBMFont* addTitle(const char* text, const char* font = "goldFont.fnt");
        cocos2d::CCLabelBMFont* addSubtitle(const char* text, bool centered = false);
        gd::CCMenuItemSpriteExtra* addButton(cocos2d::CCNode* sprite, cocos2d::SEL_MenuHandler callback, bool large = false);

        void onInfo(cocos2d::CCObject*);
        void onPage(cocos2d::CCObject*);
        void onToggle(cocos2d::CCObject*);
        void textChanged(gd::CCTextInputNode*) override;

        void onShowAccount(cocos2d::CCObject*);
        void onURL(cocos2d::CCObject*);

    public:
        static BESettingsLayer* create();
};
