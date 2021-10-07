#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x18c8e0, "_ZN12LoadingLayer10loadAssetsEv")
void __fastcall LoadingLayer_loadAssets(LoadingLayer* self) {
    GDMAKE_ORIG(self);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto label = CCLabelBMFont::create("Loading BetterEdit Textures...", "goldFont.fnt");

    label->setPosition(winSize.width / 2, winSize.height / 2 - 30.0f);
    label->setScale(.5f);

    self->addChild(label, 999);

    BetterEdit::sharedState()->loadTextures();
    
    label->removeFromParent();
}

