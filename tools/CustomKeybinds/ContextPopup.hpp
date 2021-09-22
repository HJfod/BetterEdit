#pragma once

#include "../../BetterEdit.hpp"
#include "SuperMouseManager.hpp"
#include "SuperKeyboardManager.hpp"

enum ContextPopupDirection {
    kContextPopupDirectionLeft,
    kContextPopupDirectionRight,
    kContextPopupDirectionUp,
    kContextPopupDirectionDown,
};

enum ContextPopupType {
    kContextPopupTypeBlack,
    kContextPopupTypeBrown,
};

class ContextPopup :
    public CCNodeRGBA,
    public SuperMouseDelegate,
    public SuperKeyboardDelegate
{
    protected:
        ContextPopupType m_eType;
        ContextPopupDirection m_eDirection;
        CCSize m_obSize;
        CCScale9Sprite* m_pBG;
        CCSprite* m_pThumb;
        CCPoint m_obOrigin;
        CCPoint m_obDestination;
        bool m_bAnimationComplete = false;

        virtual void setup() = 0;
        void mouseLeaveSuper(CCPoint const&) override;
        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;
        bool keyDownSuper(enumKeyCodes);
        void mouseScrollOutsideSuper(float, float) override;
        bool mouseScrollSuper(float, float) override;

        bool init(
            CCPoint const&,
            CCSize const&,
            ContextPopupDirection,
            ContextPopupType
        );
    
    public:
        virtual void onAnimationComplete();
        virtual void onHide();

        void show();
        void hide();
};
