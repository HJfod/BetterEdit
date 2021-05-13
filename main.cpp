#include "src/BetterEdit.hpp"
#include <Windows.h>

DWORD WINAPI unload_thread(void* hModule) {
    be::unload();

    FreeLibraryAndExitThread((HMODULE)hModule, 0);

    exit(0);

    return 0;
}

DWORD WINAPI load_thread(void* hModule) {
    if (!be::init()) {
        MessageBoxA(nullptr, "BetterEdit: Unable to initialize!", "Error", MB_ICONERROR);

        be::unload();

        FreeLibraryAndExitThread((HMODULE)hModule, 0);

        return 0;
    }

    #ifdef GDCONSOLE
    be::awaitUnload();

    unload_thread(hModule);
    #endif

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0x1000, load_thread, hModule, 0, 0);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH: break;
        case DLL_PROCESS_DETACH:
            #ifndef GDCONSOLE
            CreateThread(0, 0x1000, unload_thread, hModule, 0, 0);
            #endif
            break;
    }
    
    return true;
}

