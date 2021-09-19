// generated through GDMake https://github.com/HJfod/GDMake

// include GDMake & submodules
#include <GDMake.h>
#include "BetterEdit.hpp"
#include "tools/Templates/templates.hpp"
#include "tools/Favourites/favourite.hpp"
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
#include "tools/CustomKeybinds/KeybindManager.hpp"
#include "tools/CustomKeybinds/BEKeybinds.hpp"
#include "tools/CustomKeybinds/SuperKeyboardManager.hpp"
#include "tools/CustomKeybinds/SuperMouseManager.hpp"
#include "tools/other/placeObjectsBefore.hpp"
#include "tools/CustomUI/UIManager.hpp"
#include "tools/History/UndoHistoryManager.hpp"

GDMAKE_MAIN_HM(hMod) {
    BetterEdit::log() << "Loading BetterEdit" << log_end();
    BetterEdit::log() << "Applying patches" << log_end();

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
    patch(0x23b56, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

    BetterEdit::log() << "Initializing UIManager" << log_end();
    if (!UIManager::initGlobal())
        return "Unable to initialize UIManager!";

    BetterEdit::log() << "Initializing BetterEdit" << log_end();
    if (!BetterEdit::initGlobal())
        return "Unable to initialize BetterEdit!";

    BetterEdit::log() << "Initializing LiveManager" << log_end();
    if (!LiveManager::initGlobal())
        return "Unable to initialize LiveManager!";

    BetterEdit::log() << "Initializing SoftSaveManager" << log_end();
    if (!SoftSaveManager::initGlobal())
        return "Unable to initialize SoftSaveManager!";

    BetterEdit::log() << "Initializing LevelBackupManager" << log_end();
    if (!LevelBackupManager::initGlobal())
        return "Unable to initialize LevelBackupManager!";

    BetterEdit::log() << "Initializing KeybindManager" << log_end();
    if (!KeybindManager::initGlobal())
        return "Unable to initialize KeybindManager!";

    BetterEdit::log() << "Initializing SuperKeyboardManager" << log_end();
    if (!SuperKeyboardManager::initGlobal())
        return "Unable to initialize SuperKeyboardManager!";

    BetterEdit::log() << "Initializing SuperMouseManager" << log_end();
    if (!SuperMouseManager::initGlobal())
        return "Unable to initialize SuperMouseManager!";

    BetterEdit::log() << "Initializing UndoHistoryManager" << log_end();
    if (!UndoHistoryManager::initGlobal())
        return "Unable to initialize UndoHistoryManager!";

    BetterEdit::log() << "Creating midhooks" << log_end();
    if (!loadUpdateVisibilityHook())
        return "Unable to midhook updateVisibility!";
    if (!loadDrawGridLayerMidHook())
        return "Unable to midhook DrawGridLayer::draw!";

    BetterEdit::log() << "Loading tools" << log_end();
    loadBEKeybinds();
    loadTemplates();
    loadFavouriteTab();
    loadEnterSearch();
    loadTeleportScaleFix();
    loadFLAlertLayerFix();
    loadPlaceObjectsBefore();

    // checkForUpdates();

    BetterEdit::log() << "Initializing GDMake hooks" << log_end();

    return "";
}

GDMAKE_UNLOAD {
    BetterEdit::log() << "Unloading BetterEdit" << log_end();

    BetterEdit::log() << "Unpatching Addresses" << log_end();

    // unpatch all addresses
    unpatch(0);
    unloadEnterSearch();

    BetterEdit::log() << "Uninitializing GDMake" << log_end();
}
