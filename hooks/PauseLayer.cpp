#include <GDMake.h>

using namespace gdmake;

GDMAKE_HOOK(0x1e60e0, "_ZN10PauseLayer6onEditEPN7cocos2d8CCObjectE")
void __fastcall PauseLayer_onEdit(cocos2d::CCLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    // this fixes the scale buttons for
    // some reason idk
    self->removeFromParentAndCleanup(true);
}
