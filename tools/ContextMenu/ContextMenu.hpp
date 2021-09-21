#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../CustomKeybinds/SuperKeyboardManager.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"

class ContextMenu;

#define ANYLABEL_TYPE(_type_)               \
    protected:                              \
    _type_* m_p##_type_ = nullptr;          \
    inline bool init(_type_* p) {           \
        if (!CCNodeRGBA::init())            \
            return false;                   \
        this->m_p##_type_ = p;              \
        this->addChild(p);                  \
        this->setCascadeColorEnabled(true); \
        this->setCascadeOpacityEnabled(true);\
        return true;                        \
    }                                       \
    public:                                 \
    static AnyLabel* create(_type_* p) {    \
        auto ret = new AnyLabel;            \
        if (ret && ret->init(p)) {          \
            ret->autorelease(); return ret; \
        }                                   \
        CC_SAFE_DELETE(ret); return nullptr;\
    }                                       \

class AnyLabel : public CCNodeRGBA, public CCLabelProtocol {
        ANYLABEL_TYPE(CCLabelBMFont);
        ANYLABEL_TYPE(CCLabelTTF);

    public:
        inline void setString(const char* s) override {
            if (m_pCCLabelBMFont)
                m_pCCLabelBMFont->setString(s);
            if (m_pCCLabelTTF)
                m_pCCLabelTTF->setString(s);
        }
        inline const char* getString(void) override {
            if (m_pCCLabelBMFont)
                return m_pCCLabelBMFont->getString();
            if (m_pCCLabelTTF)
                return m_pCCLabelTTF->getString();
        }
};

class ContextMenuItem :
    public CCNode,
    public SuperMouseDelegate
{
    protected:
        CCPoint m_obLastMousePos;
        ContextMenu* m_pMenu;
        
        friend class ContextMenu;

        bool init();
        void draw() override;
        bool mouseDownSuper(MouseButton btn, CCPoint const&) override;
        bool mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        virtual void activate();
        virtual void drag(float);
};

class SpecialContextMenuItem : public ContextMenuItem {
    public:
        using Callback = std::function<void(SpecialContextMenuItem*)>;

    protected:
        CCSprite* m_pSprite;
        Callback m_pCallback;
        CCLabelTTF* m_pLabel;

        bool init(const char*, const char*, Callback);
        void activate() override;
        void visit() override;

    public:
        static SpecialContextMenuItem* create(
            const char* spr, const char* txt, Callback
        );
};

class KeybindContextMenuItem : public ContextMenuItem {
    protected:
        keybind_id m_sID;
        KeybindCallback* m_pCallback;
        CCLabelTTF* m_pLabel;

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

class ContextMenu :
    public CCLayerColor,
    public SuperMouseDelegate,
    public SuperKeyboardDelegate
{
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
        void keyDownSuper(enumKeyCodes) override;

        void draw() override;

    public:
        static ContextMenu* create();
        static ContextMenu* load();
        static ContextMenu* get();
        
        void show();
        void show(CCPoint const& pos);
        void hide();
};
