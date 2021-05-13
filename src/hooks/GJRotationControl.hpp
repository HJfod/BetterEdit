#pragma once

#include "../offsets.hpp"

class GJRotationControl : cocos2d::CCLayer {
    protected:
        inline static bool (__thiscall* init)(GJRotationControl*);
        static bool __fastcall initHook(GJRotationControl*);

        static inline bool (__thiscall* ccTouchBegan_)(GJRotationControl*, cocos2d::CCTouch*, cocos2d::CCEvent*);
        static bool __fastcall ccTouchBeganHook(GJRotationControl*, edx_t, cocos2d::CCTouch*, cocos2d::CCEvent*);

    public:
        static bool loadHook() {
            MH_CreateHook(
                (PVOID)(gd::base + 0x94200),
                as<LPVOID>(GJRotationControl::ccTouchBeganHook),
                as<LPVOID*>(&GJRotationControl::ccTouchBegan_)
            );

            return MH_CreateHook(
                (PVOID)(gd::base + 0x93f10),
                as<LPVOID>(GJRotationControl::initHook),
                as<LPVOID*>(&GJRotationControl::init)
            ) == MH_OK;
        }
};
