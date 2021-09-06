#pragma once

#include "../../BetterEdit.hpp"

enum MouseButton {
    kMouseButtonLeft    = 0,
    kMouseButtonRight   = 1,
    kMouseButtonMiddle  = 2,
    kMouseButtonNext    = 3,
    kMouseButtonPrev    = 4,
};

class SuperMouseManager;

class SuperMouseDelegate : public CCNode {
    protected:
        CCSize m_obSuperMouseHitSize = CCSizeZero;
        bool m_bSuperMouseHovered = false;

        SuperMouseDelegate();
        ~SuperMouseDelegate();

        friend class SuperMouseManager;

    public:
        virtual void mouseEnterSuper(CCPoint const&);
        virtual void mouseLeaveSuper(CCPoint const&);
        virtual void mouseDownSuper(MouseButton, CCPoint const&);
        virtual void mouseUpSuper(MouseButton, CCPoint const&);
        virtual void mouseMoveSuper(CCPoint const&);
        virtual void setSuperMouseHitSize(CCSize const&);
};

class SuperMouseManager {
    protected:
        std::vector<SuperMouseDelegate*> m_vDelegates;
        CCPoint m_obLastPosition;

        bool init();

        bool delegateIsHovered(SuperMouseDelegate*, CCPoint const&);
    
    public:
        static bool initGlobal();
        static SuperMouseManager* get();

        void pushDelegate(SuperMouseDelegate*);
        void popDelegate(SuperMouseDelegate*);

        void dispatchClickEvent(MouseButton, bool, CCPoint const&);
        void dispatchMoveEvent(CCPoint const&);
};

