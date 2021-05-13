#pragma once

#include "../offsets.hpp"

class BrownAlertDelegate : public gd::FLAlertLayer {
    protected:
        cocos2d::CCSize m_pLrSize;

        virtual bool init(float, float, const char* = "GJ_square01.png", const char* = "");
        virtual void setup() = 0;

        inline void onClose(cocos2d::CCObject*);
};

