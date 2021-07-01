#include "../BetterEdit.hpp"

GDMAKE_HOOK(0x1dc920)
void ObjectToolbox_gridNodeSizeForKey(int objID) {
    if (!BetterEdit::sharedState()->getKeyInt("grid-size-enabled"))
        return GDMAKE_ORIG_V(objID);

    float gridSize = static_cast<float>(BetterEdit::sharedState()->getKeyInt("grid-size"));
    __asm { movss xmm0, gridSize }
}
