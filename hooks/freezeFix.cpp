// thank you cos8o
// adapted from https://github.com/cos8oih/editorFreezeFix

#include <GDMake.h>
#include "passTouch.hpp"

using namespace gdmake;

void __fastcall EditorUI_onPlayTest(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    if (!g_bHoldingDownTouch)
        GDMAKE_ORIG_V(self, edx, pSender);
}
