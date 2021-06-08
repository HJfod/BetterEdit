// generated through GDMake https://github.com/HJfod/GDMake

// include GDMake & submodules
#include <GDMake.h>

GDMAKE_MAIN {
    // main entrypoint for your mod.
    // this is where you do things like
    // create hooks, load settings, etc.

    // you don't have to enable hooks, as
    // they are automatically enabled after
    // this method.

    // gdmake::patchBytes(0x1e62a6,
    //     {
    //         0x90, 0x90,                                 // PUSH 0x0
    //         0x90, 0x90, 0x90, 0x90, 0x90,               // PUSH PauseLayer::goEdit
    //         0x90,                                       // PUSH EDI
    //         0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
    //         0x90, 0x90, 0x90,                           // ADD ESP, 0x8
    //         0x90,                                       // PUSH EAX
    //         0x90,                                       // PUSH ECX
    //         0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,   // MOV dword ptr
    //         0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
    //         0x90, 0x90, 0x90,                           // ADD ESP, 0x4
    //         0x90,                                       // PUSH EAX
    //         0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
    //         0x90, 0x90, 0x90,                           // ADD ESP, 0xC
    //         0x90, 0x90,                                 // MOV ECX, EDI
    //         0x90,                                       // PUSH EAX
    //         0x90, 0x90, 0x90, 0x90, 0x90, 0x90,         // CALL dword ptr
    //     }
    // );

    // gdmake::patchBytes(0x1e62a6, {
    //     0x8b, 0xcf,                     // MOV ECX, EDI
    //     0xe8, 0x43, 0x00, 0x00, 0x00    // CALL PauseLayer::goEdit
    // });

    return true;
}

GDMAKE_UNLOAD {
    // gdmake::patchBytes(0x1e62a6,
    //     {
    //         0x6a, 0x00,                                 // PUSH 0x0
    //         0x68, 0xf0, 0x62, 0x5e, 0x00,               // PUSH PauseLayer::goEdit
    //         0x57,                                       // PUSH EDI
    //         0xff, 0x15, 0x6c, 0x22, 0x68, 0x00,         // CALL dword ptr
    //         0x83, 0xc4, 0x08,                           // ADD ESP, 0x8
    //         0x50,                                       // PUSH EAX
    //         0x51,                                       // PUSH ECX
    //         0xc7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,   // MOV dword ptr
    //         0xff, 0x15, 0x7c, 0x22, 0x68, 0x00,         // CALL dword ptr
    //         0x83, 0xc4, 0x04,                           // ADD ESP, 0x4
    //         0x50,                                       // PUSH EAX
    //         0xff, 0x15, 0x68, 0x22, 0x68, 0x00,         // CALL dword ptr
    //         0x83, 0xc4, 0x0c,                           // ADD ESP, 0xC
    //         0x8b, 0xcf,                                 // MOV ECX, EDI
    //         0x50,                                       // PUSH EAX
    //         0xff, 0x15, 0x88, 0x22, 0x68, 0x00,         // CALL dword ptr
    //     }
    // );

    // if you need to do some extra cleanup
    // for your mod, write it here.
    
    // all default submodules are automatically
    // dealt with afterwards.
}
