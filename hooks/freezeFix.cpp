#include <GDMake.h>
#include "passTouch.hpp"

using namespace gdmake;

// thank you cos8o
// adapted from https://github.com/cos8oih/editorFreezeFix

GDMAKE_HOOK(0x87600)
void __fastcall EditorUI_onPlayTest(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    self->m_bTouchDown = false;
    if (!g_bHoldingDownTouch)
        GDMAKE_ORIG_V(self, edx, pSender);
}
