#include <GDMake.h>
#include "BetterEdit.hpp"

class EditorPauseLayer : public gd::CCBlockLayer {
    public:
        PAD(0x8)
        gd::CCMenuItemSpriteExtra* m_pButton0;
        gd::CCMenuItemSpriteExtra* m_pButton1;
        gd::LevelEditorLayer* m_pEditorLayer;
};

GDMAKE_HOOK(0x730e0)
bool __fastcall EditorPauseLayer_init(EditorPauseLayer* self, gdmake::edx_t edx, gd::LevelEditorLayer* el) {
    if (!GDMAKE_ORIG(self, edx, el))
        return false;

    auto menu = gdmake::extra::as<cocos2d::CCMenu*>(self->m_pButton0->getParent());

    auto btn = gd::CCMenuItemSpriteExtra::create(
        gd::ButtonSprite::create(
            "BetterEdit Settings",
            0, 0, "bigFont.fnt", "GJ_button_01.png",
            0, .8f
        ),
        self,
        nullptr
    );

    btn->setPosition(0, cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2 - 40);

    menu->addChild(btn);

    return true;
}
