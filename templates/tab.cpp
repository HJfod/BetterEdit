
#include <GDMake.h>
#include "TemplateMenu.hpp"

using namespace gdmake;

class EditorUI_CB : public gd::EditorUI {
    public:
        void showTemplateMenu(cocos2d::CCObject*) {
            extra::as<TemplateMenu*>(this->m_pCreateButtonBar)->switchMode(TemplateMenu::kTModeEdit);
        }
};

GDMAKE_HOOK(0x7caf0)
void __fastcall EditorUI_setupCreateMenu(gd::EditorUI* self) {
    GDMAKE_ORIG_V(self);

    auto newTab_off = cocos2d::CCSprite::createWithSpriteFrameName("GJ_tabOff_001.png");
    auto off_aspr = cocos2d::CCSprite::createWithSpriteFrameName("puzzle_base_001.png");

    off_aspr->setScale(.35f);
    off_aspr->setPosition(newTab_off->getScaledContentSize() / 2);

    newTab_off->addChild(off_aspr);

    newTab_off->setOpacity(150);

    auto newTab_on = cocos2d::CCSprite::createWithSpriteFrameName("GJ_tabOn_001.png");

    newTab_on->addChild(off_aspr);

    auto newTab = gd::CCMenuItemToggler::create(
        newTab_off,
        newTab_on,
        self,
        (cocos2d::SEL_MenuHandler)&gd::EditorUI::onSelectBuildTab
    );

    newTab->setSizeMult(1.2f);
    newTab->setClickable(false);
    newTab->setContentSize(
        extra::as<cocos2d::CCNode*>(self->m_pTabsArray->objectAtIndex(0))->getContentSize()
    );

    newTab->setTag(self->m_pTabsArray->count());

    auto btns = cocos2d::CCArray::create();
    
    btns->addObject(gd::CCMenuItemSpriteExtra::create(
        make_bspr('+'),
        self,
        (cocos2d::SEL_MenuHandler)&EditorUI_CB::showTemplateMenu
    ));

    btns->addObject(gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png"),
        self,
        nullptr
    ));

    auto bbar = TemplateMenu::create(
        btns, { cocos2d::CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f }, self->m_pTabsArray->count(), false,
        gd::GameManager::sharedState()->getIntGameVariable("0049"),
        gd::GameManager::sharedState()->getIntGameVariable("0050")
    );

    bbar->setVisible(false);

    self->m_pTabsArray->addObject(newTab);
    self->m_pTabsMenu->addChild(newTab);
    self->m_pCreateButtonBars->addObject(bbar);
    self->addChild(bbar, 10);

    auto pos = self->m_pTabsMenu->getPosition();

    self->m_pTabsMenu->alignItemsHorizontallyWithPadding(-3.0f);

    self->m_pTabsMenu->setPosition(pos);
}
