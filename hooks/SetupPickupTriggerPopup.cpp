#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x23d550, "_ZN23SetupPickupTriggerPopup4initEP16EffectGameObjectPN7cocos2d7CCArrayE")
bool __fastcall SetupPickupTriggerPopup_init(SetupPickupTriggerPopup* self, edx_t edx, EffectGameObject* obj, CCArray* arr) {
    if (!GDMAKE_ORIG(self, edx, obj, arr))
        return false;
    
    self->m_pCountInput->setAllowedChars("0123456789-");

    return true;
}
