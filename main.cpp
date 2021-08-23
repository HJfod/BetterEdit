// generated through GDMake https://github.com/HJfod/GDMake

// include GDMake & submodules
#include <GDMake.h>
#include "BetterEdit.hpp"
#include "tools/Templates/templates.hpp"
#include "tools/Favourites/favourite.hpp"
#include "tools/ContextMenu/ContextMenu.hpp"
#include "tools/LiveCollab/LiveManager.hpp"
#include "tools/EditorLayerInput/LayerManager.hpp"
#include "tools/EditorLayerInput/editorLayerInput.hpp"
#include "tools/EnterToSearch/loadEnterSearch.hpp"
#include "tools/EyeDropper/eyeDropper.hpp"
#include "tools/AutoSave/autoSave.hpp"
#include "tools/AutoSave/Backup/LevelBackupManager.hpp"
#include "tools/other/teleportScaleFix.hpp"
#include "tools/AutoUpdate/autoUpdate.hpp"
#include "tools/PortalLineColors/portalLineColors.hpp"
#include "tools/FLAlertLayerFix/FLAlertLayerFix.hpp"

GDMAKE_MAIN_HM(hMod) {
    patch(0x1e62a6,
        {
            0x8b, 0xcf,                                 // MOV ECX, EDI
            0xe8, 0x43, 0x00, 0x00, 0x00,               // CALL PauseLayer::goEdit
            0x90,                                       // PUSH EDI
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
            0x90, 0x90, 0x90,                           // ADD ESP, 0x8
            0x90,                                       // PUSH EAX
            0x90,                                       // PUSH ECX
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,   // MOV dword ptr
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
            0x90, 0x90, 0x90,                           // ADD ESP, 0x4
            0x90,                                       // PUSH EAX
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
            0x90, 0x90, 0x90,                           // ADD ESP, 0xC
            0x90, 0x90,                                 // MOV ECX, EDI
            0x90,                                       // PUSH EAX
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
        }
    );

    // thanks to adaf
    // this enables pulses in FMODAudioEngine in every layer, instead of just in PlayLayer
    patchBytes(0x23b56, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

    if (!BetterEdit::initGlobal())
        return "Unable to initialize BetterEdit!";
    if (!LiveManager::initGlobal())
        return "Unable to initialize LiveManager!";
    if (!LayerManager::initGlobal())
        return "Unable to initialize LayerManager!";
    if (!SoftSaveManager::initGlobal())
        return "Unable to initialize SoftSaveManager!";
    if (!LevelBackupManager::initGlobal())
        return "Unable to initialize LevelBackupManager!";

    if (!loadUpdateVisibilityHook())
        return "Unable to midhook updateVisibility!";
    if (!loadDrawGridLayerMidHook())
        return "Unable to midhook DrawGridLayer::draw!";

    loadTemplates();
    loadFavouriteTab();
    loadEnterSearch();
    loadTeleportScaleFix();
    loadFLAlertLayerFix();

    ContextMenu::loadRightClick(hMod);

    // checkForUpdates();

    return "";
}

GDMAKE_UNLOAD {
    ContextMenu::unloadRightClick();

    // unpatch all addresses
    unpatch(0);
    unloadEnterSearch();
}
