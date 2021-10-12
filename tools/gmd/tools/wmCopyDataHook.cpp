#include "../../../BetterEdit.hpp"
#include "passGmdFile.hpp"
#include "../layers/goToHook.hpp"

GDMAKE_HOOK("libcocos2d.dll" + 0x113090)
LRESULT __stdcall CCEGLView_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        auto data = as<PCOPYDATASTRUCT>(lParam);

        BetterEdit::log() << "Received Data From Another Instance" << log_end();

        if (data->dwData == MSG_GD_IMPORT_LEVEL) {
            auto str = as<const char*>(data->lpData);

            auto lvls = stringSplit(str, GD_IMPORT_SEPARATOR);

            BetterEdit::log()
                << "Received " << lvls.size()
                << " Levels, Adding to Import List" << log_end();
            
            addLevelsToImportLayerFromVector(lvls);

            SetForegroundWindow(getGDHWND());
            BringWindowToTop(getGDHWND());
            SetFocus(getGDHWND());
            SetActiveWindow(getGDHWND());
        }

        return TRUE;
    }

    return GDMAKE_ORIG(hwnd, msg, wParam, lParam);
}
