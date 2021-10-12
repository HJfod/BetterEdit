#include "goToHook.hpp"
#include "ImportLayer.hpp"
#include "ImportObject.hpp"
#include "../logic/gmd.hpp"
#include "../../Notifications/NotificationManager.hpp"
#include "../../Debug/IntegratedConsole.hpp"

CCArray* g_pList = nullptr;
std::vector<std::string> g_vErrors;
bool g_bLoadedGame = false;

void notifyImportErrors(ImportLayer* layer) {
    if (g_vErrors.size()) {
        std::string text = "";
        bool color = false;
        for (auto const& err : g_vErrors) {
            text += (color ? "<cr>" : "<cg>") + err + "</c>\n ";
            color = !color;
        }
        text = text.substr(0, text.size() - 2);
        NotificationManager::get()->schedule(
            Notification::create(
                kNotificationTypeError, "There were some errors while importing levels",
                [text]() -> void {
                    // IntegratedConsole::create()->show();
                    FLAlertLayer::create(
                        nullptr,
                        "Import Errors",
                        "OK", nullptr,
                        360.f,
                        text
                    )->show();

                    g_vErrors.clear();
                }, layer
            )
        );
    }
}

void goToImportLayer(CCArray* list) {
    g_pList = list;
    g_pList->retain();

    if (g_bLoadedGame && !ImportLayer::isOpen()) {
        notifyImportErrors(ImportLayer::scene(g_pList, true));
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
        notifyImportErrors(layer);
        layer->addItemsToList(list);
        list->release();
    } else {
        goToImportLayer(list);
    }
}

void notifyErrorsWhileImporting(std::vector<std::string> const& v) {
    g_vErrors.insert(g_vErrors.begin(), v.begin(), v.end());

    if (g_bLoadedGame) {
        for (auto error : g_vErrors) {
            NotificationManager::get()->schedule(
                Notification::create(kNotificationTypeError, error)
            );
        }
        g_vErrors.clear();
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
            g_vErrors.push_back(
                "Unable to Import "_s + lvl + ": " + res.error
            );
            
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
        notifyImportErrors(ImportLayer::scene(g_pList));
        return g_pList->release();
    }

    GDMAKE_ORIG(self);
}
