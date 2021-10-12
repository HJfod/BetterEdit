#include "passGmdFile.hpp"

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto found = reinterpret_cast<HWND*>(lParam);

    // buffer long enough to hold "Geometry Dash"
    char name[16];
    GetWindowTextA(hwnd, name, 16);

    // buffer long enough to hold "GLFW30"
    char className[16];
    GetClassNameA(hwnd, className, 16);

    if (strcmp(name, "Geometry Dash") == 0 && strcmp(className, "GLFW30") == 0) {
        // check that window isn't the current one

        auto gd_wnd = getGDHWND();
        if (gd_wnd == hwnd) {
            return TRUE;
        }

        // check when the window was created??
        // somehow??

        *found = hwnd;
        return FALSE;
    }
    
    return TRUE;
}

HWND anotherInstanceIsOpen() {
    HWND found = nullptr;

    EnumWindows(&EnumWindowsProc, as<LPARAM>(&found));

    return found;
}
