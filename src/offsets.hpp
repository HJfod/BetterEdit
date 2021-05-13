#pragma once

#ifndef INCLUDE_SUBMODULES
    #define INCLUDE_SUBMODULES
    #pragma warning( push, 0 )
    #include <cocos2d.h>
    #include <GUI/CCControlExtension/CCScale9Sprite.h>
    #include <MinHook.h>
    #include <gd.h>
    #pragma warning( pop )
#endif

using offset_type = const uintptr_t;
#define GDCONSOLE

static constexpr const cocos2d::ccColor3B listBGLight { 194, 114, 62 };
static constexpr const cocos2d::ccColor3B listBGDark  { 161, 88,  44 };

template<typename T>
static T getChild(cocos2d::CCNode* x, int i) {
    return static_cast<T>(x->getChildren()->objectAtIndex(i));
}

template <typename ret>
ret ReadChain(std::vector<uintptr_t>_offs, ret _if_fucky = ret()) {
    uintptr_t* read = reinterpret_cast<std::uintptr_t*>(_offs.at(0));
    bool not_valid = false;

    for (unsigned int i = 1; i < _offs.size() - 1; i++)
        if (read == nullptr) {
            not_valid = true; break;
        } else
            read = reinterpret_cast<std::uintptr_t*>(*read + _offs.at(i));
    
    if (!not_valid)
        return *(ret*)(*read + _offs[_offs.size() - 1]);
    else return _if_fucky;
}

template <typename T, typename R>
T as(R const v) { return reinterpret_cast<T>(v); }

inline std::string operator"" _s (const char* _txt, size_t) {
    return std::string(_txt);
}

template<typename T, typename U> constexpr size_t offsetOf(U T::*member) {
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

inline bool patchBytes(
    uintptr_t const address,
    std::vector<uint8_t> const& bytes
) {
    return WriteProcessMemory(
        GetCurrentProcess(),
        reinterpret_cast<LPVOID>(gd::base + address),
        bytes.data(),
        bytes.size(),
        nullptr
    );
}

// thanks pie
inline bool patchVoid(void* dst, const void* src, void* buff, const int len) {
	if (buff)
		patchVoid(buff, dst, nullptr, len);
    
	return WriteProcessMemory(GetCurrentProcess(), dst, src, len, nullptr);
}

// thanks pie
inline bool patchVFunction(
    uintptr_t const address,
    void* const func
) {
    DWORD old;
    auto addr = &func;

    VirtualProtect((LPVOID)(gd::base + address), 4, PAGE_EXECUTE_READWRITE, &old);
    auto res = patchVoid((void*)(gd::base + address), &addr, nullptr, 4);
    VirtualProtect((LPVOID)(gd::base + address), 4, PAGE_READONLY, &old);

    return res;
}

using unknown_t = uintptr_t;
using edx_t = uintptr_t;

typedef const char* nullstr_t;
static constexpr nullstr_t nullstr = "";
