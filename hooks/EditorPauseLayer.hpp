#pragma once

#include "../BetterEdit.hpp"

class EditorPauseLayer_CB : public gd::EditorPauseLayer {
    public:
        void onBESettings(cocos2d::CCObject* pSender);
        void onRotateSaws(CCObject* pSender);
};

void setupRotateSaws();
