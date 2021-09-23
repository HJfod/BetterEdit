#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../CustomKeybinds/SuperKeyboardManager.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"

class ContextMenu;
class CustomizeCMLayer;

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
        bool m_bDisabled = false;
        
        friend class ContextMenu;
        friend class CustomizeCMLayer;

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

class PropContextMenuItem :
    public ContextMenuItem,
    public SuperKeyboardDelegate
{
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
        float m_fDragCollect = 0;
        std::function<float(GameObject*)> m_getValue;
        std::function<void(GameObject*, float, bool)> m_setValue;
        std::function<std::string()> m_getName;
        bool m_bEditingText = false;
        CCTextInputNode* m_pInput;

        float getValue();
        void setValue(float, bool = false);

        void enableInput(bool);

        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool keyDownSuper(enumKeyCodes) override;

        bool init(ContextMenu*, Type);
        void drag(float) override;
        void visit() override;
        void draw() override;

        virtual ~PropContextMenuItem();

    public:
        static PropContextMenuItem* create(ContextMenu*, Type);
};

struct ContextMenuStorageItem {
    enum ItemType {
        kItemTypeKeybind,
        kItemTypeProperty,
    };
    int m_nGrow = 1;
    ItemType m_eType;
    keybind_id m_sKeybindID;
    PropContextMenuItem::Type m_ePropType;
    ContextMenuStorageItem(const char* id, int grow = 1) {
        this->m_eType = kItemTypeKeybind;
        this->m_sKeybindID = id;
        this->m_nGrow = grow;
    }
    ContextMenuStorageItem(PropContextMenuItem::Type type, int grow = 1) {
        this->m_eType = kItemTypeProperty;
        this->m_ePropType = type;
        this->m_nGrow = grow;
    }
};

class ContextMenu :
    public CCLayerColor,
    public SuperMouseDelegate,
    public SuperKeyboardDelegate
{
    public:
        enum State {
            kStateAuto = -1,
            kStateNoneSelected,
            kStateOneSelected,
            kStateManySelected,
            kStateContextAware,
        };

        inline static std::string stateToString(State state) {
            switch (state) {
                case kStateNoneSelected: return "Empty Selection";
                case kStateOneSelected: return "One Object Selected";
                case kStateManySelected: return "Multiple Selected";
                case kStateContextAware: return "Context-Aware";
                default: return "Unknown State";
            }
        }

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
        struct Context {
            std::set<int> ids;
            Config config;

            inline Context(Config c) {
                this->ids = {};
                this->config = c;
            }
            inline Context(std::set<int> i, Config c) {
                this->ids = i;
                this->config = c;
            }
        };

    protected:
        CCPoint m_obLocation;
        LevelEditorLayer* m_pEditor;
        bool m_bDrawBorder = false;
        bool m_bDisabled = false;
        std::unordered_map<State, Config> m_mConfig;
        std::unordered_map<std::string, Context> m_mContexts;
        const char* m_sFont = "Segoe UI";
        float m_fTTFFontSize = 28.f;
        float m_fFontScale = .2f;
        AnyLabelType m_eType = kAnyLabelTypeTTF;
        int m_nAnimationSpeed = 7;

        friend class ContextMenuItem;
        friend class SpecialContextMenuItem;
        friend class KeybindContextMenuItem;
        friend class PropContextMenuItem;
        friend class CustomizeCMLayer;

        bool init() override;

        void generate(State = kStateAuto);
        void updatePosition();

        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool keyDownSuper(enumKeyCodes) override;

        void draw() override;

    public:
        static ContextMenu* create();
        static ContextMenu* load();
        static ContextMenu* get();

        inline void setDisabled(bool b) {
            this->m_bDisabled = b;
        }
        
        void show();
        void show(CCPoint const& pos);
        void hide();
};
