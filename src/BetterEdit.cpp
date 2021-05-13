#include "BetterEdit.hpp"
#include "layers/EditorUI.hpp"
#include "console.hpp"
#include "hooks/GJRotationControl.hpp"

bool be::init() {
    if (MH_Initialize() != MH_OK)
        return false;
    
    if (EditorUI::load() != MH_OK)
        return false;
    
    GJRotationControl::loadHook();

    MH_EnableHook(MH_ALL_HOOKS);

    #ifdef GDCONSOLE
    gd::console::load();
    #endif

    return true;
}

void be::unload() {
    #ifdef GDCONSOLE
    gd::console::unload();
    #endif

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

#ifdef GDCONSOLE
void be::awaitUnload() {
    std::string inp;
    getline(std::cin, inp);

    if (inp != "e")
        be::awaitUnload();
}
#endif

