#include "BEKeybinds.hpp"
#include "../AutoSave/autoSave.hpp"

void loadBEKeybinds() {
    KeybindManager::get()->addEditorKeybind({ "Cancel Autosave", [](EditorUI* self) -> bool {
        if (getAutoSaveTimer(self)->cancellable()) {
            getAutoSaveTimer(self)->cancel();
            return true;
        }
        
        return false;
    }}, {{ KEY_Escape, 0 }});
}
