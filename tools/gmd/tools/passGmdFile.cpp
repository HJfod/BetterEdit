#include "passGmdFile.hpp"

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto found = reinterpret_cast<HWND*>(lParam);

    TCHAR buffer[MAX_PATH] = {0};
    DWORD dwProcId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcId);

    GetModuleFileName((HMODULE)dwProcId, buffer, MAX_PATH);

    std::string exePath = buffer;

    // buffer long enough to hold "Geometry Dash"
    char name[16];
    GetWindowTextA(hwnd, name, 16);

    std::cout << "title: " << name << " -> " << buffer << "\n";

    if (exePath.ends_with("GeometryDash.exe")) {
        // check that window isn't the current one
        auto gd_wnd = getGDHWND();
        if (gd_wnd == hwnd) {
            return TRUE;
        }

        std::cout << "found\n";
        *found = hwnd;
        // return FALSE;
    }

    return TRUE;
}

HWND anotherInstanceIsOpen() {
    HWND found = nullptr;

    std::cout << "enmummeratin\n";
    EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&found));

    return found;
}
