#include <GDMake.h>
#include "AdvancedFilterLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"

using namespace gdmake;

static bool g_bUndid = false;

GDMAKE_HOOK(0x86250, "_ZN8EditorUI12selectObjectEP10GameObjectb")
void __fastcall EditorUI_selectObject(EditorUI* self, edx_t edx, GameObject* obj, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;
    
    if (AdvancedFilterLayer::testSelectObject(obj) && testSelectObjectLayer(obj)) {
        GDMAKE_ORIG_V(self, edx, obj, idk);
    }
}

GDMAKE_HOOK(0x864a0, "_ZN8EditorUI13selectObjectsEPN7cocos2d7CCArrayEb")
void __fastcall EditorUI_selectObjects(EditorUI* self, edx_t edx, cocos2d::CCArray* objs, bool idk) {
    if (BetterEdit::isEditorViewOnlyMode())
        return;

    if (!g_bUndid) {
        CCARRAY_FOREACH_B_BASE(objs, obj, GameObject*, ix) {
            if (!AdvancedFilterLayer::testSelectObject(obj) ||
                !testSelectObjectLayer(obj))
                objs->removeObjectAtIndex(ix--);
        }
    }
    
    GDMAKE_ORIG_V(self, edx, objs, idk);
}

GDMAKE_HOOK(0x87070, "_ZN8EditorUI14undoLastActionEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_undoLastAction(EditorUI* self, edx_t edx, CCObject* pSender) {
    g_bUndid = true;
    GDMAKE_ORIG_V(self, edx, pSender);
    g_bUndid = false;
}
