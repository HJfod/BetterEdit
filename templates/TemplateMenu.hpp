#pragma once

#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

inline cocos2d::CCSprite* make_bspr(char c) {
    auto spr = cocos2d::CCSprite::create("GJ_button_01.png");

    auto label = cocos2d::CCLabelBMFont::create(std::string(1, c).c_str(), "bigFont.fnt");
    label->setPosition(spr->getContentSize().width / 2, spr->getContentSize().height / 2 + 3.0f);

    spr->addChild(label);

    return spr;
}

class TemplateMenu : public gd::EditButtonBar {
    public:
        enum TMode {
            kTModeNormal,
            kTModeEdit,
        };

    protected:
        TMode m_nMode;
        cocos2d::CCSize m_obSize;
        cocos2d::CCMenu* m_pEditMenu;
        bool init(cocos2d::CCArray*, int, bool, int, int, cocos2d::CCPoint);

        void setupEditMenu();

        void onSaveEdit(cocos2d::CCObject*);
        void onCancelEdit(cocos2d::CCObject*);

    public:
        static TemplateMenu* create(cocos2d::CCArray*, cocos2d::CCPoint, int, bool, int, int);

        void switchMode(TMode);
};
