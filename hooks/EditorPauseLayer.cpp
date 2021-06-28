#include <GDMake.h>

using namespace gdmake;

GDMAKE_HOOK(0x75660)
void __fastcall EditorPauseLayer_onExitEditor(cocos2d::CCLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    self->removeFromParentAndCleanup(true);
}
