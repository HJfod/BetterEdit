#include "../../BetterEdit.hpp"

GDMAKE_HOOK("libcocos2d.dll" + 0x113090)
LRESULT __stdcall CCEGLView_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        std::cout << "WM_COPYDATA\n";
        return TRUE;
    }

    return GDMAKE_ORIG(hwnd, msg, wParam, lParam);
}
