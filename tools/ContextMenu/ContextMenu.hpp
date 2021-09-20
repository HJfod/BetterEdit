#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"

class ContextMenuItem :
    public CCNode,
    public SuperMouseDelegate
{
    protected:
        CCPoint m_obLastMousePos;

        bool init();
        void draw() override;
        void mouseDownSuper(MouseButton btn, CCPoint const&) override;
        void mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        virtual void activate();
        virtual void drag(int);
};

class KeybindContextMenuItem : public ContextMenuItem {
    protected:
        keybind_id m_sID;

        bool init(keybind_id const&);
        void activate() override;
    
    public:
        static KeybindContextMenuItem* create(keybind_id const&);
};

class PropertyContextMenuItem : public ContextMenuItem {
    public:
        enum Type {
            kTypeScale,
            kTypeRotate,
        };

    protected:
        Type m_eType;
        CCLabelBMFont* m_pValueLabel;

        bool init(Type);
        void drag(int) override;

    public:
        static PropertyContextMenuItem* create(Type);
};

class ContextMenu : public CCLayerColor, public SuperMouseDelegate {
    protected:
        bool m_bDrawBorder = false;

        bool init() override;

        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;

        void draw() override;

    public:
        static ContextMenu* create();
        static ContextMenu* load();
        static ContextMenu* get();
        
        void show();
        void show(CCPoint const& pos);
        void hide();
};
