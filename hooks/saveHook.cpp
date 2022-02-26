#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x3D5E0, "_ZN11AppDelegate11trySaveGameEv")
void __fastcall AppDelegate_trySaveGame(cocos2d::CCObject* self) {
    BetterEdit::sharedState()->save();

    BetterEdit::log() << kDebugTypeSaving << "Saving Finished" << log_end();

    return GDMAKE_ORIG_V(self);
}
