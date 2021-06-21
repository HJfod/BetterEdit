#include <GDMake.h>
#include "../tools/Presets/browserHook.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x6f5d0)
bool __fastcall EditLevelLayer_init(gd::EditLevelLayer* self, edx_t edx, gd::GJGameLevel* level) {
    if (!GDMAKE_ORIG(self, edx, level))
        return false;

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    auto garageRope = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("garageRope_001.png"),
        self,
        (cocos2d::SEL_MenuHandler)&gd::LevelInfoLayer::onGarage
    );
    garageRope->setSizeMult(1.2f);
    garageRope->useAnimationType(gd::kMenuAnimationTypeMove);
    garageRope->setPosition(winSize.width / 2 - 80.0f, winSize.height / 2);
    garageRope->setOffset({ .2f, .2f });
    garageRope->setDestination({ 0.0f, -8.0f });

    self->m_pSomeMenu->addChild(garageRope);

    setupCreatePresetButton(self, level);

    return true;
}
