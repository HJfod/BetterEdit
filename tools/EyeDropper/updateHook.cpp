#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "updateHook.hpp"
#include <mutex>

bool g_bLoadedResources = false;
std::map<int, bool> g_bPressedButtons;

bool isLeftMouseButtonPressed() {
    return g_bPressedButtons[0];
}

GDMAKE_HOOK("libcocos2d.dll::?onGLFWMouseCallBack@CCEGLView@cocos2d@@IAEXPAUGLFWwindow@@HHH@Z") GDMAKE_ATTR(NoLog)
void __fastcall CCEGLView_onGLFWMouseCallBack(CCEGLView* self, edx_t edx, GLFWwindow* wnd, int btn, int pressed, int z) {
    g_bPressedButtons[btn] = pressed;

    KeybindManager::get()->registerMousePress(static_cast<MouseButton>(btn), pressed);

    if (SuperMouseManager::get()->dispatchClickEvent(
        static_cast<MouseButton>(btn), pressed, getMousePos()
    ))
        return;

    if (LevelEditorLayer::get()) {
        KeybindManager::get()->executeEditorCallbacks(
            Keybind(static_cast<MouseButton>(btn)),
            LevelEditorLayer::get()->getEditorUI(),
            pressed
        );
    }
    
    if (PlayLayer::get()) {
        KeybindManager::get()->executePlayCallbacks(
            Keybind(static_cast<MouseButton>(btn)),
            PlayLayer::get(),
            pressed
        );
    }

    return GDMAKE_ORIG_V(self, edx, wnd, btn, pressed, z);
}
