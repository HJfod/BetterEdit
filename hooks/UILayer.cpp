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

GDMAKE_HOOK(0x25f3b0)
bool __fastcall UILayer_init(UILayer* self) {
    if (!GDMAKE_ORIG(self))
        return false;

    if (!self->m_pCheckPointMenu)
        return true;

    auto zBind = KeybindManager::get()->getTargetByID("gd.play.place_checkpoint");
    std::string z = "";
    for (auto b : KeybindManager::get()->getKeybindsForCallback(zBind.type, zBind.bind))
        z += b.toString() + ", ";
    z = z.substr(0, z.size() - 2);
    
    auto btnZ = getChild<CCMenuItemSpriteExtra*>(self->m_pCheckPointMenu, 0);
    if (!btnZ)
        return true;

    auto zLabel = getChild<CCLabelBMFont*>(btnZ->getNormalImage(), 0);
    if (!zLabel)
        return true;
    
    zLabel->setString(z.c_str());
    zLabel->limitLabelWidth(60.0f, zLabel->getScale(), .1f);

    auto yBind = KeybindManager::get()->getTargetByID("gd.play.delete_checkpoint");
    std::string y = "";
    for (auto b : KeybindManager::get()->getKeybindsForCallback(yBind.type, yBind.bind))
        y += b.toString() + ", ";
    y = y.substr(0, y.size() - 2);

    auto btnY = getChild<CCMenuItemSpriteExtra*>(self->m_pCheckPointMenu, 1);
    if (!btnY)
        return true;
    
    auto yLabel = getChild<CCLabelBMFont*>(btnY->getNormalImage(), 0);
    if (!yLabel)
        return true;

    yLabel->setString(y.c_str());
    yLabel->limitLabelWidth(60.0f, yLabel->getScale(), .1f);

    return true;
}
