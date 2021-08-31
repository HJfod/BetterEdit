#pragma once

#include "KeybindManager.hpp"
#include "loadEditorKeybindIndicators.hpp"

GDMAKE_HOOK("libcocos2d.dll::?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z") GDMAKE_ATTR(NoLog)
void __fastcall dispatchKeyboardMSGHook(
    CCKeyboardDispatcher* self,
    edx_t edx,
    enumKeyCodes key,
    bool down
) {
    KeybindManager::get()->registerKeyPress(key, down);

    GDMAKE_ORIG_V(self, edx, key, down);
}

GDMAKE_HOOK(0x6ebc0)
void __fastcall EditButtonBar_goToPage(EditButtonBar* self, edx_t edx, int page) {
    GDMAKE_ORIG_V(self, edx, page);

    updateEditorKeybindIndicators();
}

GDMAKE_HOOK(0x6ec20)
void __fastcall EditButtonBar_onRight(EditButtonBar* self, edx_t edx, CCObject* obj) {
    GDMAKE_ORIG_V(self, edx, obj);

    updateEditorKeybindIndicators();
}

GDMAKE_HOOK(0x6ec80)
void __fastcall EditButtonBar_onLeft(EditButtonBar* self, edx_t edx, CCObject* obj) {
    GDMAKE_ORIG_V(self, edx, obj);

    updateEditorKeybindIndicators();
}
