#include "../../BetterEdit.hpp"
#include "PresetLayer.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x15cbf0)
void __fastcall LevelBrowserLayer_onNew(gd::LevelBrowserLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    PresetLayer::create()->show();

    // GDMAKE_ORIG_V(self, edx, pSender);
}
