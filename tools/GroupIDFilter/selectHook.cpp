#include <GDMake.h>
#include "GroupIDInputLayer.hpp"

using namespace gdmake;

bool matchObject(gd::GameObject* obj) {
    auto groups = GroupIDInputLayer::getGroupFilter();
    auto colors = GroupIDInputLayer::getColorFilter();

    auto gids = obj->getGroupIDs();
    unsigned char pass = 0;
    if (groups->match(std::vector<int> ( gids.begin(), gids.end() )))
        pass++;
    
    if (colors->match(std::vector<int> { obj->getBaseColor()->colorID }))
        pass++;
    
    if (obj->getDetailColor()) {
        if (colors->match(std::vector<int> { obj->getDetailColor()->colorID }))
            pass++;
    } else pass++;

    return pass == 3;
}

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(gd::EditorUI* self, edx_t edx, gd::GameObject* obj, bool idk) {
    if (GroupIDInputLayer::noFilters())
        return GDMAKE_ORIG_V(self, edx, obj, idk);
    
    if (matchObject(obj))
        return GDMAKE_ORIG_V(self, edx, obj, idk);
}

GDMAKE_HOOK(0x864a0)
void __fastcall EditorUI_selectObjects(gd::EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (GroupIDInputLayer::noFilters())
        return GDMAKE_ORIG_V(self, edx, objs, idk);

    CCARRAY_FOREACH_B_BASE(objs, obj, gd::GameObject*, ix)
        if (!matchObject(obj))
            objs->removeObject(obj);
    
    return GDMAKE_ORIG_V(self, edx, objs, idk);
}
