#include "addTab.hpp"
#include "../BetterEdit.hpp"

using namespace gdmake;

std::vector<std::tuple<std::string, addEditorTabFunc>> g_tabs;

void addEditorTab(const char* spr, addEditorTabFunc bbar) {
    g_tabs.push_back({ spr, bbar });
}

GDMAKE_HOOK(0x7caf0)
void __fastcall EditorUI_setupCreateMenu(gd::EditorUI* self) {
    GDMAKE_ORIG_V(self);

    if (BetterEdit::isEditorViewOnlyMode())
        return;

    auto pos = self->m_pTabsMenu->getPosition();

    for (auto [spr, bbarf] : g_tabs) {
        auto newTab_off = cocos2d::CCSprite::createWithSpriteFrameName("GJ_tabOff_001.png");
        auto off_aspr = cocos2d::CCSprite::createWithSpriteFrameName(spr.c_str());

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

        auto bbar = bbarf(self);

        bbar->setVisible(false);

        self->m_pTabsArray->addObject(newTab);
        self->m_pTabsMenu->addChild(newTab);
        self->m_pCreateButtonBars->addObject(bbar);
        self->addChild(bbar, 10);
    }

    // fix F1 & F2
    patch(0x92044, { 0xb9, static_cast<uint8_t>(self->m_pTabsArray->count()) });
    patch(0x9207a, { 0x83, 0xf8, static_cast<uint8_t>(self->m_pTabsArray->count() - 1) });

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;

    if (ratio > 1.5f)
        self->m_pTabsMenu->alignItemsHorizontallyWithPadding(-3.0f);
    else
        self->m_pTabsMenu->alignItemsHorizontallyWithPadding(-1.0f);

    self->m_pTabsMenu->setPosition(pos);
}
