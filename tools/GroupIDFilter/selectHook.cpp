#include <GDMake.h>
#include "GroupIDInputLayer.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(gd::EditorUI* self, edx_t edx, gd::GameObject* obj, bool idk) {
    auto groups = GroupIDInputLayer::getGroupFilter();
    auto colors = GroupIDInputLayer::getColorFilter();

    if (!(groups.size() || colors.size()))
        return GDMAKE_ORIG_V(self, edx, obj, idk);
    
    bool pass = true;
    for (auto id : obj->getGroupIDs())
        for (auto g_id : groups)
            if (id != g_id)
                pass = false;
    
    for (auto c_id : colors)
        if (obj->getBaseColor()->colorID != c_id || obj->getDetailColor()->colorID != c_id)
            pass = false;
    
    if (pass)
        GDMAKE_ORIG_V(self, edx, obj, idk);
}
