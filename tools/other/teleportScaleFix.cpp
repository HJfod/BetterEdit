#include "teleportScaleFix.hpp"

// i have no idea where to place this function
void __fastcall TeleportPortalobject_setRScale(TeleportPortalObject* self, void*, float scale) {
    // stupid c++ wont let me call gd::GameObject::setRScale directly
    reinterpret_cast<void(__thiscall*)(GameObject*, float)>(gd::base + 0xe5280)(self, scale);
    
    if (self->m_pOrangePortal) {
        self->m_pOrangePortal->m_fScale = self->m_fScale;
        self->m_pOrangePortal->setRScale(1.f);
    }
}

void fixPortalScale(GameObject* obj) {
    if (obj->m_nObjectID == 747) {
        auto op = as<TeleportPortalObject*>(obj)->m_pOrangePortal;
        // idk why rob uses a negative scale instead of just flip x
        op->m_fScale = obj->m_fScale;
        op->setRScale(1.f);
    }
}

void loadTeleportScaleFix() {
    patch(0x2e3e50, intToBytes(as<uintptr_t>(&TeleportPortalobject_setRScale)));
}
