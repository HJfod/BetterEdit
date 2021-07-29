#include "../BetterEdit.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"

GDMAKE_HOOK(0x15ee00)
bool __fastcall LevelEditorLayer_init(LevelEditorLayer* self, edx_t edx, GJGameLevel* level) {
    if (!GDMAKE_ORIG(self, edx, level))
        return false;
    
    updatePercentLabelPosition(self->m_pEditorUI);

    return true;
}
