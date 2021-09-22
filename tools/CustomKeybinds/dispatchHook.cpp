#pragma once

#include "KeybindManager.hpp"
#include "loadEditorKeybindIndicators.hpp"
#include "SuperKeyboardManager.hpp"
#include "SuperMouseManager.hpp"

GDMAKE_HOOK("libcocos2d.dll::?dispatchScrollMSG@CCMouseDispatcher@cocos2d@@QAE_NMM@Z") GDMAKE_ATTR(NoLog)
bool __fastcall dispatchScrollMSGHook(CCMouseDelegate* self, edx_t edx, float y, float x) {
    if (SuperMouseManager::get()->dispatchScrollEvent(y, x, getMousePos()))
        return true;

    return GDMAKE_ORIG(self, edx, y, x);
}

GDMAKE_HOOK("libcocos2d.dll::?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z") GDMAKE_ATTR(NoLog)
void __fastcall dispatchKeyboardMSGHook(
    CCKeyboardDispatcher* self,
    edx_t edx,
    enumKeyCodes key,
    bool down
) {
    if (key == KEY_Tab && self->getAltKeyPressed())
        self->updateModifierKeys(false, false, false, false);

    SuperKeyboardManager::get()->dispatchEvent(key, down);
    
    KeybindManager::get()->registerKeyPress(key, down);

    GDMAKE_ORIG_V(self, edx, key, down);
}

GDMAKE_HOOK(0x3d130)
void __fastcall AppDelegate_applicationWillEnterForeground(CCApplication* self) {
    GDMAKE_ORIG_V(self);
    
    CCDirector::sharedDirector()->getKeyboardDispatcher()
        ->updateModifierKeys(false, false, false, false);
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
