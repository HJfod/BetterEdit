#pragma once

#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "TemplateManager.hpp"

inline cocos2d::CCSprite* make_bspr(char c) {
    auto spr = cocos2d::CCSprite::create("GJ_button_01.png");

    auto label = cocos2d::CCLabelBMFont::create(std::string(1, c).c_str(), "bigFont.fnt");
    label->setPosition(spr->getContentSize().width / 2, spr->getContentSize().height / 2 + 3.0f);

    spr->addChild(label);

    return spr;
}

class TemplateButton : public gd::CCMenuItemSpriteExtra {
    protected:
        cocos2d::CCSprite* m_pSpr;
    
    public:
        static TemplateButton* create(TBlock const&, TemplateMenu*, bool);

        friend class TemplateMenu;
};

class TemplateMenu : public gd::EditButtonBar {
    public:
        enum TMode {
            kTModeNormal,
            kTModeEdit,
            kTModeEditVariants,
        };

    protected:
        const float offsetTop = 40.0f;

        Template* m_pTemplate;
        TMode m_nMode;
        cocos2d::CCSize m_obSize;
        cocos2d::CCMenu* m_pEditMenu;
        cocos2d::CCMenu* m_pVariantsMenu;
        cocos2d::CCMenu* m_pVariantsBtns;
        gd::CCMenuItemSpriteExtra* m_pVariantsCloseBtn;
        bool init(cocos2d::CCArray*, int, bool, int, int, cocos2d::CCPoint);

        void setupEditMenu();

        void onSaveEdit(cocos2d::CCObject*);
        void onCancelEdit(cocos2d::CCObject*);
        void onCloseVariantMenu(cocos2d::CCObject*);
        void onShowVariantMenu(cocos2d::CCObject*);
        void onSaveBlock(cocos2d::CCObject*);
        void onSaveBlockVariant(cocos2d::CCObject*);

        friend class TemplateButton;

    public:
        static TemplateMenu* create(cocos2d::CCArray*, cocos2d::CCPoint, int, bool, int, int);

        void showVariantMenu(std::vector<TBlock>);
        void switchMode(TMode);
};
