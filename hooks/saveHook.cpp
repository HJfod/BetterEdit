#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x3D5E0)
void __fastcall AppDelegate_trySaveGame(cocos2d::CCObject* self) {
    BetterEdit::sharedState()->save();

    return GDMAKE_ORIG_V(self);
}
