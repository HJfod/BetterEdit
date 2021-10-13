#include "levelBrowserHook.hpp"

GDMAKE_HOOK(0x15a040)
bool __fastcall LevelBrowserLayer_init(LevelBrowserLayer* self, edx_t edx, GJSearchObject* sobj) {
    if (!GDMAKE_ORIG(self, edx, sobj))
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (sobj->getType() == kGJSearchTypeMyLevels) {
        auto menu = getChild<CCMenu*>(self, 4);

        auto normSearchBtn = getChild<CCNode*>(menu, 5);

        auto searchBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
            self,
            menu_selector(LevelBrowserLayer_CB::onFilterSearch)
        );
        searchBtn->setColor(cc3x(0xff0));
        searchBtn->setPosition(
            normSearchBtn->getPositionX(),
            normSearchBtn->getPositionY() - 40.f
        );
        menu->addChild(searchBtn);
    }

    return true;
}
