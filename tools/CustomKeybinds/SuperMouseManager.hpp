#pragma once

#include "../../BetterEdit.hpp"
#include "KeybindManager.hpp"

class SuperMouseManager;

class SuperMouseDelegate {
    protected:
        CCPoint m_obSuperMouseHitOffset = CCPointZero;
        CCSize m_obSuperMouseHitSize = CCSizeZero;
        bool m_bSuperMouseHovered = false;
        bool m_bSuperMouseDown = false;

        SuperMouseDelegate();
        virtual ~SuperMouseDelegate();

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
        virtual void setSuperMouseHitOffset(CCPoint const&);
};

class SuperMouseManager {
    protected:
        std::vector<SuperMouseDelegate*> m_vDelegates;
        CCPoint m_obLastPosition;
        SuperMouseDelegate* m_pCapturing = nullptr;

        bool init();

        bool delegateIsHovered(SuperMouseDelegate*, CCPoint const&);
    
    public:
        static bool initGlobal();
        static SuperMouseManager* get();

        void pushDelegate(SuperMouseDelegate*);
        void popDelegate(SuperMouseDelegate*);

        void captureMouse(SuperMouseDelegate*);
        void releaseCapture(SuperMouseDelegate*);
        void releaseCapture();

        bool dispatchClickEvent(MouseButton, bool, CCPoint const&);
        void dispatchMoveEvent(CCPoint const&);
        bool dispatchScrollEvent(float, float, CCPoint const&);
};

