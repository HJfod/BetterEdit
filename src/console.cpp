#include "console.hpp"
#include <Windows.h>
#include <stdio.h>

bool gd::console::load() {
    if (AllocConsole() == 0)
        return false;

    // redirect console output
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONERR$", "w", stderr);

    return true;
}

void gd::console::unload() {
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    FreeConsole();
}

