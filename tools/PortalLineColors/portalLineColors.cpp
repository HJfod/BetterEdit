#include "portalLineColors.hpp"

#define MAKE_MIDHOOK(addr, func)        \
    if (MH_CreateHook(                  \
        as<LPVOID>(gd::base + addr),    \
        as<LPVOID>(func##_midHook),     \
        as<LPVOID*>(&func##_retAddr)    \
    ) != MH_OK) return false;           \
                                        \
    if (MH_EnableHook(                  \
        as<LPVOID>(gd::base + addr)     \
    ) != MH_OK)                         \
        return false;
    
void DrawGridLayer_draw() {
    GameObject* obj;

    __asm {
        mov obj, eax
    }

    if (!BetterEdit::getDontColorGamemodeBorders())
        switch (obj->m_nObjectID) {
            case 12:  ccDrawColor4B(88,  255, 100, 255); break; // cube
            case 13:  ccDrawColor4B(255, 150, 255, 255); break; // ship
            case 47:  ccDrawColor4B(255, 34,  0,   255); break; // ball
            case 111: ccDrawColor4B(255, 214, 85,  255); break; // ufo
            case 660: ccDrawColor4B(31,  221, 255, 255); break; // wave
            case 745: ccDrawColor4B(222, 221, 225, 255); break; // robot
            case 1331:ccDrawColor4B(116, 21,  255, 255); break; // spider
        }
}

void (*DrawGridLayer_draw_retAddr)();
__declspec(naked) void DrawGridLayer_draw_midHook() {
    __asm {
        pushad
        pushfd
        call DrawGridLayer_draw
        popfd
        popad
        jmp DrawGridLayer_draw_retAddr
    }
}

bool loadDrawGridLayerMidHook() {
    MAKE_MIDHOOK(0x16db5d, DrawGridLayer_draw);

    return true;
}
