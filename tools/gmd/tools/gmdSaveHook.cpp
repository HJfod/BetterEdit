#include "../logic/GmdSaveManager.hpp"

GDMAKE_HOOK(0x18e040)
void __fastcall LocalLevelManager_encodeDataTo(
    LocalLevelManager* self, edx_t edx, DS_Dictionary* dict
) {
    if (true ||!BetterEdit::getSaveLevelsAsGmd()) {
        return GDMAKE_ORIG_V(self, edx, dict);
    }

    dict->setStringForKey(
        "be-info",
        "Local Levels are currently being saved in the " +
        GmdSaveManager::get()->getDirName() + " directory. "
        "You can change this behaviour in BetterEdit settings."
    );
    GmdSaveManager::get()->save(self);
}

GDMAKE_HOOK(0x18e070)
void __fastcall LocalLevelManager_dataLoaded(
    LocalLevelManager* self, edx_t edx, DS_Dictionary* dict
) {
    if (true ||!BetterEdit::getSaveLevelsAsGmd()) {
        return GDMAKE_ORIG_V(self, edx, dict);
    }

    if (!dict->getStringForKey("be-info").size()) {
        GmdSaveManager::get()->backupCCLocalLevels(dict);
    }
    GDMAKE_ORIG_V(self, edx, dict);
    GmdSaveManager::get()->load(self);
}
