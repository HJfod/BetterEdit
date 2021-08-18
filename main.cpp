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

// i have no idea where to place this function
void __fastcall TeleportPortalobject_setRScale(gd::TeleportPortalObject* self, void*, float scale) {
    // stupid c++ wont let me call gd::GameObject::setRScale directly
    reinterpret_cast<void(__thiscall*)(gd::GameObject*, float)>(gd::base + 0xe5280)(self, scale);
    if (self->m_pOrangePortal) {
        self->m_pOrangePortal->m_fScale = self->m_fScale;
        self->m_pOrangePortal->setRScale(1.f);
    }
}

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

    if (!BetterEdit::initGlobal())
        return false;
    if (!LiveManager::initGlobal())
        return false;
    if (!LayerManager::initGlobal())
        return false;
    if (!SoftSaveManager::initGlobal())
        return false;
    if (!LevelBackupManager::initGlobal())
        return false;

    if (!loadUpdateVisibilityHook())
        return false;
    
    loadTemplates();
    loadFavouriteTab();
    loadEnterSearch();

    // CCTextureCache::sharedTextureCache()
        // ->addImage("BE_GameSheet01.png", false);
    // CCSpriteFrameCache::sharedSpriteFrameCache()
        // ->addSpriteFramesWithFile("BE_GameSheet01.plist");

    ContextMenu::loadRightClick(hMod);

    patch(0x2e3e50, intToBytes(as<uintptr_t>(&TeleportPortalobject_setRScale)));

    return true;
}

GDMAKE_UNLOAD {
    ContextMenu::unloadRightClick();

    // unpatch all addresses
    unpatch(0);
    unloadEnterSearch();
}
