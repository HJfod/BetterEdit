#include "../../BetterEdit.hpp"
#include "eyeDropper.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"
#include "../CustomKeybinds/loadEditorKeybindIndicators.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../Tutorial/tutorial.hpp"
#include "updateHook.hpp"
#include <mutex>

bool g_bLoadedResources = false;
std::map<int, bool> g_bPressedButtons;
std::vector<std::function<void()>> g_vGDThreadFuncs;
std::mutex g_thread;

bool isLeftMouseButtonPressed() {
    return g_bPressedButtons[0];
}

void scheduleOneTimeFuncInGDThread(std::function<void()> f) {
    g_vGDThreadFuncs.push_back(f);
}

GDMAKE_HOOK("libcocos2d.dll::?onGLFWMouseCallBack@CCEGLView@cocos2d@@IAEXPAUGLFWwindow@@HHH@Z") GDMAKE_ATTR(NoLog)
void __fastcall CCEGLView_onGLFWMouseCallBack(CCEGLView* self, edx_t edx, GLFWwindow* wnd, int btn, int pressed, int z) {
    if (handleEyeDropperClick(btn, pressed))
        return;

    if (showingTutorial())
        return showNextTutorialPage();

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

GDMAKE_HOOK("libcocos2d.dll::?update@CCScheduler@cocos2d@@UAEXM@Z") GDMAKE_ATTR(NoLog)
void __fastcall CCScheduler_update(CCScheduler* self, edx_t edx, float dt) {
    updateEyeDropperColor();
    
    // debug stuff (has to be done in the GD thread, so i
    // cant just do this in GDMAKE_MAIN)
    if (GDMAKE_IS_CONSOLE_ENABLED && !g_bLoadedResources) {
        g_bLoadedResources = true;
        BetterEdit::sharedState()->loadTextures();
    }

    for (auto const& func : g_vGDThreadFuncs) {
        g_thread.lock();
        func();
        g_thread.unlock();
    }

    KeybindManager::get()->handleRepeats(dt);
    SuperMouseManager::get()->dispatchMoveEvent(getMousePos());

    showEditorKeybindIndicatorIfItsTargetIsBeingHovered();

    return GDMAKE_ORIG_V(self, edx, dt);
}
