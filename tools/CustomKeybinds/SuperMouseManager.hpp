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

class SuperMouseDelegate {
    protected:
        CCSize m_obSuperMouseHitSize = CCSizeZero;
        bool m_bSuperMouseHovered = false;

        SuperMouseDelegate();
        ~SuperMouseDelegate();

        friend class SuperMouseManager;

    public:
        virtual void mouseEnterSuper(CCPoint const&);
        virtual void mouseLeaveSuper(CCPoint const&);
        virtual bool mouseDownSuper(MouseButton, CCPoint const&);
        virtual bool mouseUpSuper(MouseButton, CCPoint const&);
        virtual void mouseMoveSuper(CCPoint const&);
        virtual void mouseDownOutsideSuper(MouseButton, CCPoint const&);
        virtual bool mouseScrollSuper(float y, float x);
        virtual void mouseScrollOutsideSuper(float y, float x);
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

        bool dispatchClickEvent(MouseButton, bool, CCPoint const&);
        void dispatchMoveEvent(CCPoint const&);
        bool dispatchScrollEvent(float, float, CCPoint const&);
};

