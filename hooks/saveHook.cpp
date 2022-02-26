#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x3D5E0, "_ZN11AppDelegate11trySaveGameEv")
void __fastcall AppDelegate_trySaveGame(cocos2d::CCObject* self) {
    BetterEdit::log() << kDebugTypeSaving << "Saving GD" << log_end();
    GDMAKE_ORIG_V(self);
    BetterEdit::log() << kDebugTypeSaving << "Saving BetterEdit" << log_end();
    BetterEdit::sharedState()->save();
    BetterEdit::log() << kDebugTypeSaving << "Saving Finished" << log_end();
}
