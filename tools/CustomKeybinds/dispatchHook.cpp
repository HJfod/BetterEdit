#pragma once

#include "KeybindManager.hpp"

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
