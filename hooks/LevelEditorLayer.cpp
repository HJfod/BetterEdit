#include "../BetterEdit.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"
#include "../tools/RotateSaws/rotateSaws.hpp"
#include "../tools/AutoSave/autoSave.hpp"

GDMAKE_HOOK(0x15ee00)
bool __fastcall LevelEditorLayer_init(LevelEditorLayer* self, edx_t edx, GJGameLevel* level) {
    if (!GDMAKE_ORIG(self, edx, level))
        return false;

    BetterEdit::setEditorInitialized(true);

    if (SoftSaveManager::hasLoad(level->levelName)) {
        auto layer = FLAlertLayer::create(
            SoftSaveAlertDelegate::create(),
            "Unsaved Work Detected",
            "Cancel", "Load",
            340.0f,
            "It appears the game unexpectedly closed and you have "
            "unsaved work. <cy>Would you like to recover it?</c>"
        );

        layer->m_pTargetLayer = self->m_pEditorUI;
        
        layer->show();
    }
    
    updatePercentLabelPosition(self->m_pEditorUI);

    return true;
}

GDMAKE_HOOK(0x15e8d0)
void __fastcall LevelEditorLayer_destructorHook(LevelEditorLayer* self) {
    BetterEdit::setEditorInitialized(false);
    BetterEdit::setEditorViewOnlyMode(false);

    return GDMAKE_ORIG_V(self);
}

GDMAKE_HOOK(0x162650)
void __fastcall LevelEditorLayer_addSpecial(
    LevelEditorLayer* self,
    edx_t edx,
    GameObject* obj
) {
    GDMAKE_ORIG_V(self, edx, obj);

    handleObjectAddForSlider(self, obj);
    SoftSaveManager::saveObject(obj);

    if (shouldRotateSaw() && objectIsSaw(obj))
        beginRotateSaw(obj);

    if (obj->m_nObjectID == 747) {
        auto op = as<gd::TeleportPortalObject*>(obj)->m_pOrangePortal;
        // idk why rob uses a negative scale instead of just flip x
        op->m_fScale = object->m_fScale;
        op->setRScale(1.f);
    }
}

GDMAKE_HOOK(0x161cb0)
void __fastcall LevelEditorLayer_removeObject(
    LevelEditorLayer* self,
    edx_t edx,
    GameObject* obj,
    bool idk
) {
    GDMAKE_ORIG_V(self, edx, obj, idk);

    handleObjectAddForSlider(self, obj);
    SoftSaveManager::removeObject(obj);

    if (shouldRotateSaw() && objectIsSaw(obj))
        stopRotateSaw(obj);
}
