#include "../../../BetterEdit.hpp"
#include "passGmdFile.hpp"

GDMAKE_HOOK("libcocos2d.dll" + 0x113090)
LRESULT __stdcall CCEGLView_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        std::cout << "WM_COPYDATA\n";
        auto data = as<PCOPYDATASTRUCT>(lParam);

        if (data->dwData == MSG_GD_IMPORT_LEVEL) {
            std::cout << "importing deez nuts\n";

            auto arr = as<CCArray*>(data->lpData);

            CCARRAY_FOREACH_B_TYPE(arr, str, CCString) {
                std::cout << str->getCString() << "\n";
            }
        }

        return TRUE;
    }

    return GDMAKE_ORIG(hwnd, msg, wParam, lParam);
}
