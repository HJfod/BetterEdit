#include <GDMake.h>
#include "../tools/Presets/browserHook.hpp"
#include "../tools/CustomStartPos/ellHook.hpp"
#include "../tools/AutoSave/Backup/setupBackupButton.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x6f5d0)
bool __fastcall EditLevelLayer_init(EditLevelLayer* self, edx_t edx, GJGameLevel* level) {
    if (!GDMAKE_ORIG(self, edx, level))
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto garageRope = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("garageRope_001.png"),
        self,
        (SEL_MenuHandler)&LevelInfoLayer::onGarage
    );
    garageRope->setSizeMult(1.2f);
    garageRope->useAnimationType(kMenuAnimationTypeMove);
    garageRope->setPosition(winSize.width / 2 - 80.0f, winSize.height / 2);
    garageRope->setOffset({ .2f, .2f });
    garageRope->setDestination({ 0.0f, -8.0f });

    self->m_pButtonMenu->addChild(garageRope);

    setupCreatePresetButton(self, level);
    loadStartPosButton(self, level);
    setupBackupButton(self, level);

    return true;
}
