#include "../BetterEdit.hpp"
#include "../tools/CustomKeybinds/KeybindManager.hpp"

GDMAKE_HOOK(0x25f890)
void __fastcall UILayer_keyDown(UILayer* self, edx_t edx, enumKeyCodes key) {
    KeybindManager::get()->executePlayCallbacks(
        Keybind(key), PlayLayer::get(), true
    );
}

GDMAKE_HOOK(0x25fa10)
void __fastcall UILayer_keyUp(UILayer* self, edx_t edx, enumKeyCodes key) {
    KeybindManager::get()->executePlayCallbacks(
        Keybind(key), PlayLayer::get(), false
    );
}
