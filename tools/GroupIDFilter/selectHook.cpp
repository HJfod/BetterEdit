#include <GDMake.h>
#include "AdvancedFilterLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"
#include "../ContextMenu/loadContextMenu.hpp"

using namespace gdmake;

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(gd::EditorUI* self, edx_t edx, gd::GameObject* obj, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;
    
    if (shouldHideLDMObject(obj))
        return;

    SoftSaveManager::saveObject(obj);

    if (AdvancedFilterLayer::testSelectObject(obj) && testSelectObjectLayer(obj)) {
        GDMAKE_ORIG_V(self, edx, obj, idk);

        updateContextMenu();
    }
}

GDMAKE_HOOK(0x864a0)
void __fastcall EditorUI_selectObjects(gd::EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;
        
    CCARRAY_FOREACH_B_BASE(objs, obj, gd::GameObject*, ix) {
        SoftSaveManager::saveObject(obj);

        if (!AdvancedFilterLayer::testSelectObject(obj) ||
            !testSelectObjectLayer(obj) ||
            shouldHideLDMObject(obj))
            objs->removeObjectAtIndex(ix--);
    }
    
    GDMAKE_ORIG_V(self, edx, objs, idk);

    updateContextMenu();
}
