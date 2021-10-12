#include "goToHook.hpp"
#include "ImportLayer.hpp"
#include "ImportListView.hpp"
#include "../logic/gmd.hpp"

CCArray* g_pList = nullptr;
bool g_bLoadedGame = false;

void goToImportLayer(CCArray* list) {
    g_pList = list;
    g_pList->retain();

    if (g_bLoadedGame && !ImportLayer::isOpen()) {
        ImportLayer::scene(g_pList, true);
        g_pList->release();
    }
}

void addLevelsToImportLayer(CCArray* list) {
    if (!g_bLoadedGame) {
        g_pList->addObjectsFromArray(list);
        list->release();
    }
    auto layer = ImportLayer::isOpen();
    if (layer) {
        layer->addItemsToList(list);
        list->release();
    } else {
        goToImportLayer(list);
    }
}

void addLevelsToImportLayerFromVector(std::vector<std::string> const& v) {
    auto arr = CCArray::create();
    for (auto const& lvl : v) {
        auto res = gmd::GmdFile(lvl).parseLevel();
        if (res) {
            BetterEdit::log() << kDebugTypeLoading << "Added " << lvl << " to import list" << log_end();
            arr->addObject(new ImportObject(res.data, lvl));
        } else {
            BetterEdit::log() << kDebugTypeMinorError
                << "Unable to Import " << lvl << ": "
                << res.error << log_end();
        }
    }
    addLevelsToImportLayer(arr);
}

GDMAKE_HOOK(0x18c790)
void __fastcall LoadingLayer_loadingFinished(LoadingLayer* self) {
    g_bLoadedGame = true;

    if (g_pList && g_pList->count()) {
        ImportLayer::scene(g_pList);
        return g_pList->release();
    }

    GDMAKE_ORIG(self);
}
