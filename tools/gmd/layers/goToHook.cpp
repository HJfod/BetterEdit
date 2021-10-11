#include "goToHook.hpp"
#include "ImportLayer.hpp"

CCArray* g_pList = nullptr;

void goToImportLayer(CCArray* list) {
    g_pList = list;
    g_pList->retain();
}

GDMAKE_HOOK(0x18c790)
void __fastcall LoadingLayer_loadingFinished(LoadingLayer* self) {
    if (g_pList && g_pList->count()) {
        ImportLayer::scene(g_pList);
        return g_pList->release();
    }

    GDMAKE_ORIG(self);
}
