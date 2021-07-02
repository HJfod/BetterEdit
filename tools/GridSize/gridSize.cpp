#include "../../BetterEdit.hpp"

GDMAKE_HOOK(0x1dc920)
void __stdcall ObjectToolbox_gridNodeSizeForKey(int objID) {
    if (!BetterEdit::sharedState()->getGridSizeEnabled())
        return GDMAKE_ORIG_V(objID);

    float gridSize = BetterEdit::sharedState()->getGridSize();
    __asm {
        movss xmm0, gridSize
        add esp, 0x4
    }
}

GDMAKE_HOOK(0x78f60)
void __fastcall EditorUI_updateGridNodeSize(gd::EditorUI* self) {
    if (!BetterEdit::sharedState()->getGridSizeEnabled())
        return GDMAKE_ORIG_V(self);

    auto actualMode = self->m_nSelectedMode;
    self->m_nSelectedMode = gd::EditorUI::Mode_Create;

    GDMAKE_ORIG_V(self);

    self->m_nSelectedMode = actualMode;
}
