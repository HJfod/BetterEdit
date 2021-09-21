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
        if (p) this->addChild(p);           \
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

enum AnyLabelType {
    kAnyLabelTypeBM,
    kAnyLabelTypeTTF,
};

class AnyLabel : public CCNodeRGBA, public CCLabelProtocol {
        ANYLABEL_TYPE(CCLabelBMFont);
        ANYLABEL_TYPE(CCLabelTTF);

    public:
        inline void visit() override {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;

            this->setContentSize(
                label->getScaledContentSize()
            );

            CCNodeRGBA::visit();
        }
        inline void setScale(float scale) override {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;

            label->setScale(scale);
        }
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
            return "";
        }
        inline void limitLabelWidth(float width, float def, float min) {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;

            label->setScale(1.f);
            auto [cwidth, _] = label->getContentSize();
            float scale = label->getScale();
            if (width && width < cwidth) {
                scale = width / cwidth;
            }
            if (def && def < scale) {
                scale = def;
            }
            if (min && scale < min) {
                scale = min;
            }
            label->setScale(scale);
        }
        inline void limitLabelHeight(float height, float def, float min) {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;

            label->setScale(1.f);
            auto [_, cheight] = label->getContentSize();
            float scale = label->getScale();
            if (height && height < cheight) {
                scale = height / cheight;
            }
            if (def && def < scale) {
                scale = def;
            }
            if (min && scale < min) {
                scale = min;
            }
            label->setScale(scale);
        }
};

class ContextMenuItem :
    public CCNode,
    public SuperMouseDelegate
{
    protected:
        CCPoint m_obLastMousePos;
        ContextMenu* m_pMenu = nullptr;
        GLubyte m_nFade = 0;
        
        friend class ContextMenu;

        bool init(ContextMenu*);
        void draw() override;
        bool mouseDownSuper(MouseButton btn, CCPoint const&) override;
        bool mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        virtual void activate();
        virtual void drag(float);
        AnyLabel* createLabel(const char* txt = "");
};

class SpecialContextMenuItem : public ContextMenuItem {
    public:
        using Callback = std::function<bool(SpecialContextMenuItem*)>;

    protected:
        CCSprite* m_pSprite = nullptr;
        Callback m_pCallback = nullptr;
        AnyLabel* m_pLabel;
        GLubyte m_nSpriteOpacity;
        float m_fSpriteScale = .6f;

        bool init(ContextMenu*, const char*, const char*, Callback);
        void activate() override;
        void visit() override;

        friend class ContextMenu;

    public:
        static SpecialContextMenuItem* create(
            ContextMenu*,
            const char* spr,
            const char* txt,
            Callback = nullptr
        );

        void setSpriteOpacity(GLubyte);
};

class DragContextMenuItem : public SpecialContextMenuItem {
    public:
        using DragCallback = std::function<void(DragContextMenuItem*, float)>;
        using DragValue = std::function<float()>;

    protected:
        DragCallback m_pDragCallback = nullptr;
        DragValue m_pDragValue = nullptr;
        const char* m_sText;

        bool init(ContextMenu*, const char*, const char*, DragCallback, DragValue);
        void drag(float) override;
        void visit() override;

        friend class ContextMenu;

    public:
        static DragContextMenuItem* create(
            ContextMenu*,
            const char* spr,
            const char* txt,
            DragCallback = nullptr,
            DragValue = nullptr
        );
};

class KeybindContextMenuItem : public ContextMenuItem {
    protected:
        keybind_id m_sID;
        KeybindCallback* m_pCallback;
        AnyLabel* m_pLabel;

        bool init(ContextMenu*, keybind_id const&);
        void activate() override;
        void visit() override;
    
    public:
        static KeybindContextMenuItem* create(ContextMenu*, keybind_id const&);
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
        AnyLabel* m_pValueLabel;
        float m_fDragCollect = .0f;

        bool init(ContextMenu*, Type);
        void drag(float) override;

    public:
        static PropContextMenuItem* create(ContextMenu*, Type);
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

        struct Line {
            float height = 12.f;
            std::vector<ContextMenuStorageItem> items;

            inline Line(decltype(items) i) {
                items = i;
            }
            inline Line(decltype(items) i, float h) {
                items = i;
                height = h;
            }
        };
        using Config = std::vector<Line>;

    protected:
        CCPoint m_obLocation;
        LevelEditorLayer* m_pEditor;
        bool m_bDrawBorder = false;
        std::unordered_map<State, Config> m_mConfig;
        const char* m_sFont = "Segoe UI";
        float m_fTTFFontSize = 28.f;
        float m_fFontScale = .2f;
        AnyLabelType m_eType = kAnyLabelTypeTTF;
        int m_nAnimationSpeed = 7;

        friend class ContextMenuItem;
        friend class SpecialContextMenuItem;
        friend class KeybindContextMenuItem;
        friend class PropContextMenuItem;

        bool init() override;

        void generate();
        void updatePosition();

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
