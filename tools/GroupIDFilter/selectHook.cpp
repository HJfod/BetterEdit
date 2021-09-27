#include <GDMake.h>
#include "AdvancedFilterLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"
#include "../ContextMenu/loadContextMenu.hpp"

using namespace gdmake;

static bool g_bUndid = false;

GDMAKE_HOOK(0x86250)
void __fastcall EditorUI_selectObject(EditorUI* self, edx_t edx, GameObject* obj, bool idk) {
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
void __fastcall EditorUI_selectObjects(EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;

    if (!g_bUndid) {
        CCARRAY_FOREACH_B_BASE(objs, obj, GameObject*, ix) {
            SoftSaveManager::saveObject(obj);

            if (!AdvancedFilterLayer::testSelectObject(obj) ||
                !testSelectObjectLayer(obj) ||
                shouldHideLDMObject(obj))
                objs->removeObjectAtIndex(ix--);
        }
    }
    
    GDMAKE_ORIG_V(self, edx, objs, idk);

    updateContextMenu();
}

GDMAKE_HOOK(0x87070)
void __fastcall EditorUI_undoLastAction(EditorUI* self, edx_t edx, CCObject* pSender) {
    g_bUndid = true;
    GDMAKE_ORIG_V(self, edx, pSender);
    g_bUndid = false;
}
