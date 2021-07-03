#pragma once
#include <GDMake.h>

class ScaleTextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::GJScaleControl* m_pControl;
        gd::CCTextInputNode* m_pInputNode;
        gd::CCMenuItemSpriteExtra* m_pLockPosBtn;
        gd::CCMenuItemSpriteExtra* m_pLockScaleBtn;
        bool m_bLockPosEnabled;
        bool m_bUnlockScaleEnabled;

        virtual void textChanged(gd::CCTextInputNode* input) override;

        static ScaleTextDelegate* create(gd::GJScaleControl* c);
};