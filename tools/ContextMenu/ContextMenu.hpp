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
        virtual void deactivate();
        virtual void updateItem();
        virtual void drag(float);
        virtual void hide();
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

class ActionContextMenuItem : public ContextMenuItem {
    public:
        struct ACMIAction {
            std::string m_sName;
            std::function<void(ActionContextMenuItem*, GameObject*)> m_callback;
        };

        static ACMIAction actionForID(std::string const&);

    protected:
        ACMIAction m_obAction;
        AnyLabel* m_pLabel;

        bool init(ContextMenu*, std::string const&);
        void activate() override;
        void visit() override;
    
    public:
        static ActionContextMenuItem* create(ContextMenu*, std::string const&);
};

class PropContextMenuItem :
    public ContextMenuItem,
    public SuperKeyboardDelegate,
    public TextInputDelegate
{
    public:
        enum Type {
            kTypePositionX,
            kTypePositionY,
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
        float m_fDefaultValue = 0.0f;
        float m_fLastDraggedValue = 0.0f;
        std::string m_sSuffix = "";
        std::function<float(CCArray*)> m_getValue;
        std::function<void(CCArray*, float, bool)> m_setValue;
        std::function<std::string()> m_getName;
        std::function<bool()> m_usable = nullptr;
        std::function<void()> m_drawCube = nullptr;
        std::function<void()> m_undo = nullptr;
        CCSprite* m_pCube = nullptr;
        CCSprite* m_pCube2 = nullptr;
        bool m_bEditingText = false;
        bool m_bTextSelected = false;
        CCTextInputNode* m_pInput;
        int m_nCaretPos = 0;
        CCPoint m_obScaleCenter = CCPointZero;

        float getValue();
        void setValue(float, bool = false);
        bool isUsable();
        bool absoluteModifier();
        bool smallModifier();

        void enableInput(bool);

        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;
        bool mouseUpSuper(MouseButton, CCPoint const&) override;
        bool keyDownSuper(enumKeyCodes) override;
        void textChanged(CCTextInputNode*) override;

        bool init(ContextMenu*, Type);
        void drag(float) override;
        void visit() override;
        void draw() override;
        void deactivate() override;
        void updateItem() override;

        virtual ~PropContextMenuItem();

    public:
        static PropContextMenuItem* create(ContextMenu*, Type);
};

struct ContextMenuStorageItem {
    enum ItemType {
        kItemTypeKeybind,
        kItemTypeProperty,
        kItemTypeAction,
    };
    int m_nGrow = 1;
    ItemType m_eType;
    keybind_id m_sKeybindID;
    std::string m_sActionID;
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
    ContextMenuStorageItem(std::string const& id, int grow = 1) {
        this->m_eType = kItemTypeAction;
        this->m_sActionID = id;
        this->m_nGrow = grow;
    }
};

class ContextMenu :
    public CCLayerColor,
    public SuperMouseDelegate,
    public SuperKeyboardDelegate
{
    public:
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

        enum ContextType {
            kContextTypeAuto = -1,
            kContextTypeDefault,
            kContextTypeObject,
            kContextTypeSolid,
            kContextTypeDetail,
            kContextTypeTrigger,
            kContextTypeSpecial,
        };

        inline static std::string defaultContextName(ContextType type) {
            switch (type) {
                case kContextTypeDefault: return "Default";
                case kContextTypeObject: return "Object";
                case kContextTypeSolid: return "Solid";
                case kContextTypeDetail: return "Detail";
                case kContextTypeTrigger: return "Trigger";
                case kContextTypeSpecial: return "ID List";
            }
            return "Unknown";
        }

        struct Context {
            std::string m_sName = "";
            std::set<int> m_vIDs;
            Config m_vSingleConfig;
            Config m_vMultiConfig;

            inline Context() {
                this->m_vIDs = {};
            }
            inline Context(Config c) {
                this->m_vIDs = {};
                this->m_vSingleConfig = c;
            }
            inline Context(Config c, Config cm) {
                this->m_vIDs = {};
                this->m_vSingleConfig = c;
                this->m_vMultiConfig = cm;
            }
            inline Context(std::set<int> i, Config c) {
                this->m_vIDs = i;
                this->m_vSingleConfig = c;
            }
            inline Context(std::set<int> i, Config c, Config cm) {
                this->m_vIDs = i;
                this->m_vSingleConfig = c;
                this->m_vMultiConfig = cm;
            }
        };

    protected:
        CCPoint m_obLocation;
        LevelEditorLayer* m_pEditor;
        bool m_bDrawBorder = false;
        bool m_bDisabled = false;
        bool m_bSelectObjectUnderMouse = true;
        Config m_vDefaultConfig;
        std::unordered_map<ContextType, Context> m_mContexts;
        const char* m_sFont = "Segoe UI";
        float m_fTTFFontSize = 28.f;
        float m_fFontScale = .2f;
        AnyLabelType m_eType = kAnyLabelTypeTTF;
        int m_nAnimationSpeed = 7;
        GameObject* m_pObjSelectedUnderMouse = nullptr;
        bool m_bDeselectObjUnderMouse = false;

        friend class ContextMenuItem;
        friend class SpecialContextMenuItem;
        friend class KeybindContextMenuItem;
        friend class ActionContextMenuItem;
        friend class PropContextMenuItem;
        friend class CustomizeCMLayer;

        bool init() override;

        void generate(ContextType = kContextTypeAuto, bool multi = false);
        void updatePosition();
        ContextType getTypeUnderMouse(bool = true);

        void deactivateOthers(ContextMenuItem*);

        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool keyDownSuper(enumKeyCodes) override;

        void draw() override;

    public:
        static ContextMenu* create();
        static ContextMenu* load();
        static ContextMenu* get(LevelEditorLayer* = nullptr);

        void updateItems();

        inline void setDisabled(bool b) {
            this->m_bDisabled = b;
        }
        
        void show();
        void show(CCPoint const& pos);
        void hide();
};
