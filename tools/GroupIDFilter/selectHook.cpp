#include <GDMake.h>
#include "AdvancedFilterLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../AutoSave/autoSave.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(gd::EditorUI* self, edx_t edx, gd::GameObject* obj, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;

    SoftSaveManager::saveObject(obj);

    if (AdvancedFilterLayer::testSelectObject(obj) && testSelectObjectLayer(obj))
        return GDMAKE_ORIG_V(self, edx, obj, idk);
}

GDMAKE_HOOK(0x864a0)
void __fastcall EditorUI_selectObjects(gd::EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;
        
    CCARRAY_FOREACH_B_BASE(objs, obj, gd::GameObject*, ix) {
        SoftSaveManager::saveObject(obj);

        if (!AdvancedFilterLayer::testSelectObject(obj) || !testSelectObjectLayer(obj))
            objs->removeObjectAtIndex(ix--);
    }
    
    return GDMAKE_ORIG_V(self, edx, objs, idk);
}
