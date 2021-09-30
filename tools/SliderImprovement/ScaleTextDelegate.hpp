#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"

enum LockType {
    kLockTypeAbsolute,
    kLockTypeSlider,
};

class LockButton :
    public CCMenuItemSpriteExtra,
    public SuperMouseDelegate
{
    protected:
        LockType m_eType;

        bool init(CCSprite*, CCNode*, SEL_MenuHandler, LockType);

        void mouseLeaveSuper(CCPoint const&) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool mouseUpSuper(MouseButton, CCPoint const&) override;
    
    public:
        static LockButton* create(CCSprite*, CCNode*, SEL_MenuHandler, LockType);
};

class ScaleTextDelegate :
    public CCNode,
    public TextInputDelegate
{
    public:
        GJScaleControl* m_pControl;
        CCTextInputNode* m_pInputNode;
        LockButton* m_pLockPosBtn;
        LockButton* m_pLockScaleBtn;
        bool m_bLockPosEnabled;
        bool m_bUnlockScaleEnabled;

        virtual void textChanged(CCTextInputNode* input) override;

        static ScaleTextDelegate* create(GJScaleControl* c);
};
