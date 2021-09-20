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
        bool mouseDownSuper(MouseButton btn, CCPoint const&) override;
        bool mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        virtual void activate();
        virtual void drag(float);
};

class KeybindContextMenuItem : public ContextMenuItem {
    protected:
        keybind_id m_sID;
        CCLabelBMFont* m_pLabel;

        bool init(keybind_id const&);
        void activate() override;

        void visit() override;
    
    public:
        static KeybindContextMenuItem* create(keybind_id const&);
};

class PropContextMenuItem : public ContextMenuItem {
    public:
        enum Type {
            kTypeScale,
            kTypeRotate,
            kTypeZOrder,
            kTypeELayer,
            kTypeELayer2,
        };

    protected:
        Type m_eType;
        CCLabelBMFont* m_pValueLabel;
        float m_fDragCollect = .0f;

        bool init(Type);
        void drag(float) override;

    public:
        static PropContextMenuItem* create(Type);
};

struct ContextMenuStorageItem {
    enum ItemType {
        kItemTypeKeybind,
        kItemTypeProperty,
    };
    ItemType m_eType;
    keybind_id m_sKeybindID;
    PropContextMenuItem::Type m_ePropType;
    ContextMenuStorageItem(const char* id) {
        this->m_eType = kItemTypeKeybind;
        this->m_sKeybindID = id;
    }
    ContextMenuStorageItem(PropContextMenuItem::Type type) {
        this->m_eType = kItemTypeProperty;
        this->m_ePropType = type;
    }
};

class ContextMenu : public CCLayerColor, public SuperMouseDelegate {
    public:
        enum State {
            kStateNoneSelected,
            kStateOneSelected,
            kStateManySelected,
        };

        using Line = std::vector<ContextMenuStorageItem>;
        using Config = std::vector<Line>;

    protected:
        LevelEditorLayer* m_pEditor;
        bool m_bDrawBorder = false;
        std::unordered_map<State, Config> m_mConfig;

        bool init() override;

        void generate();

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
