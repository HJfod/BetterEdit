#include <GDMake.h>
#include "BetterEdit.hpp"

using namespace gdmake;

class EditorPauseLayer : public gd::CCBlockLayer {
    public:
        PAD(0x8)
        gd::CCMenuItemSpriteExtra* m_pButton0;
        gd::CCMenuItemSpriteExtra* m_pButton1;
        gd::LevelEditorLayer* m_pEditorLayer;
};

GDMAKE_HOOK(0x730e0)
bool __fastcall EditorPauseLayer_init(EditorPauseLayer* self, edx_t edx, gd::LevelEditorLayer* el) {
    if (!GDMAKE_ORIG(self, edx, el))
        return false;

    auto menu = extra::as<cocos2d::CCMenu*>(self->m_pButton0->getParent());
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto btn = gd::CCMenuItemSpriteExtra::create(
        spriteFNWithScale("GJ_optionsBtn_001.png", .6f),
        self,
        nullptr
    );

    btn->setPosition(winSize.width / 2 - 85, winSize.height - 80);

    menu->addChild(btn);

    return true;
}
