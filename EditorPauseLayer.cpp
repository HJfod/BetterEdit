#include <GDMake.h>
#include "BetterEdit.hpp"
#include "tools/settings/BESettingsLayer.hpp"

using namespace gdmake;

class EditorPauseLayer : public gd::CCBlockLayer {
    public:
        PAD(0x8)
        gd::CCMenuItemSpriteExtra* m_pButton0;
        gd::CCMenuItemSpriteExtra* m_pButton1;
        gd::LevelEditorLayer* m_pEditorLayer;

        void onBESettings(cocos2d::CCObject* pSender) {
            BESettingsLayer::create()->show();
        }
};

GDMAKE_HOOK(0x730e0)
bool __fastcall EditorPauseLayer_init(EditorPauseLayer* self, edx_t edx, gd::LevelEditorLayer* el) {
    if (!GDMAKE_ORIG(self, edx, el))
        return false;

    auto menu = extra::as<cocos2d::CCMenu*>(self->m_pButton0->getParent());
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto btn = gd::CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_optionsBtn02_001.png")
            .scale(.8f)
            .done(),
        self,
        (cocos2d::SEL_MenuHandler)&EditorPauseLayer::onBESettings
    );
    btn->setPosition(winSize.width / 2 - 70, winSize.height - 73);
    menu->addChild(btn);

    extra::getChild<cocos2d::CCMenu*>(menu, menu->getChildrenCount() - 2)
        ->setPositionX(winSize.width / 2 - 30);

    return true;
}
