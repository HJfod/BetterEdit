#include <GDMake.h>
#include "AdvancedFilterLayer.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(gd::EditorUI* self, edx_t edx, gd::GameObject* obj, bool idk) {
    if (AdvancedFilterLayer::noFilter())
        return GDMAKE_ORIG_V(self, edx, obj, idk);
    
    if (AdvancedFilterLayer::getFilter()->match(obj))
        return GDMAKE_ORIG_V(self, edx, obj, idk);
}

GDMAKE_HOOK(0x864a0)
void __fastcall EditorUI_selectObjects(gd::EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (AdvancedFilterLayer::noFilter())
        return GDMAKE_ORIG_V(self, edx, objs, idk);

    CCARRAY_FOREACH_B_BASE(objs, obj, gd::GameObject*, ix)
        if (!AdvancedFilterLayer::getFilter()->match(obj))
            objs->removeObjectAtIndex(ix--);
    
    return GDMAKE_ORIG_V(self, edx, objs, idk);
}
