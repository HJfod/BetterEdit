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
        this->setContentSize(               \
            p->getScaledContentSize()       \
        );                                  \
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

            float scale = label->getScale();
            label->setScale(1.f);
            auto [cwidth, _] = label->getContentSize();
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

            float scale = label->getScale();
            label->setScale(1.f);
            auto [_, cheight] = label->getContentSize();
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
        inline void limitLabelSize(CCSize size, float def, float min) {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;

            float scale = label->getScale();
            label->setScale(1.f);
            auto [cwidth, cheight] = label->getContentSize();
            if (size.height && size.height < cheight) {
                scale = size.height / cheight;
            }
            if (size.width && size.width < cwidth) {
                if (size.width / cwidth < scale)
                    scale = size.width / cwidth;
            }
            if (def && def < scale) {
                scale = def;
            }
            if (min && scale < min) {
                scale = min;
            }
            label->setScale(scale);
        }
        inline void setAnchorPoint(CCPoint const& pos) {
            CCNode* label = nullptr;
            if (m_pCCLabelBMFont) label = m_pCCLabelBMFont;
            if (m_pCCLabelTTF) label = m_pCCLabelTTF;
            if (!label) return;
            label->setAnchorPoint(pos);
        }
};

class ContextMenuItem;

class ContextMenuItemWithBGAndPossiblyText : public CCNode {
    protected:
        ButtonSprite* m_pButtonSprite = nullptr;
        AnyLabel* m_pLabel = nullptr;
        ccColor4F m_obColor;
        float m_fPaddingVertical = 5.f;
        float m_fPaddingHorizontal = 20.f;

        bool init(AnyLabel* label, float padh, float padv);

        void draw() override;
    
    public:
        static ContextMenuItemWithBGAndPossiblyText* create(
            AnyLabel* label, float padh = 5.f, float padv = 20.f
        );

        void setString(const char*);
        const char* getString() const;

        void updateSize();

        void setColor(ccColor4B);
};

class ContextMenuButton :
    public CCMenuItemSpriteExtra,
    public SuperMouseDelegate {
    protected:
        ContextMenuItem* m_pItem;
        ContextMenuItemWithBGAndPossiblyText* m_pBG;
        bool m_bToggled = false;
        bool m_bToggleable = false;

        bool init(ContextMenuItem*, const char* txt, SEL_MenuHandler, bool);

        bool mouseDownSuper(MouseButton btn, CCPoint const&) override;
        bool mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseLeaveSuper(CCPoint const&) override;
        void mouseEnterSuper(CCPoint const&) override;

        void visit() override;

    public:
        static ContextMenuButton* create(ContextMenuItem*, const char* txt, SEL_MenuHandler);
        static ContextMenuButton* createToggle(ContextMenuItem*, const char* txt, SEL_MenuHandler);

        void toggle(bool);
        bool isToggled() const;
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
        bool m_bHorizontalDrag = false;
        bool m_bNoHover = false;
        float m_fScrollDivisor = 6.f;
        
        friend class ContextMenu;
        friend class CustomizeCMLayer;
        friend class ContextMenuButton;

        bool init(ContextMenu*);
        void draw() override;
        bool mouseDownSuper(MouseButton btn, CCPoint const&) override;
        bool mouseUpSuper(MouseButton btn, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        bool mouseScrollSuper(float, float) override;
        virtual void activate();
        virtual void deactivate();
        virtual void updateItem();
        virtual void drag(float);
        virtual void hide();
        void updateDragDir();
    
    public:
        AnyLabel* createLabel(const char* txt = "");
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
            kTypeNone,
            kTypePositionX,
            kTypePositionY,
            kTypeScale,
            kTypeRotate,
            kTypeZOrder,
            kTypeELayer,
            kTypeELayer2,
            kTypeColor1,
            kTypeColor2,
        };

    protected:
        Type m_eType;
        AnyLabel* m_pValueLabel;
        AnyLabel* m_pNameLabel;
        AnyLabel* m_pAbsModifierLabel;
        AnyLabel* m_pSmallModifierLabel;
        float m_fDragCollect = 0;
        float m_fDefaultValue = 0.0f;
        float m_fLastDraggedValue = 0.0f;
        std::string m_sSuffix = "";
        std::function<float(CCArray*, GameObject*)> m_getValue;
        std::function<void(CCArray*, float, bool)> m_setValue;
        std::function<std::string()> m_getName;
        std::function<std::string(float)> m_specialValueName = nullptr;
        std::function<bool()> m_usable = nullptr;
        std::function<void()> m_drawCube = nullptr;
        std::function<void()> m_undo = nullptr;
        CCSprite* m_pCube = nullptr;
        CCSprite* m_pCube2 = nullptr;
        bool m_bEditingText = false;
        bool m_bTextSelected = false;
        bool m_bHasAbsoluteModifier = true;
        bool m_bOneLineText = false;
        CCTextInputNode* m_pInput;
        int m_nCaretPos = 0;
        CCPoint m_obScaleCenter = CCPointZero;
        std::unordered_map<GameObject*, float> m_mStartValues;

        virtual float getValue(GameObject* = nullptr);
        virtual void setValue(float, bool = false);
        bool isUsable();
        bool absoluteModifier();
        bool smallModifier();
        void updateStartValues();

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

class QuickGroupContextMenuItem : public ContextMenuItem {
    protected:
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;

        CCNode* hoversItem();
        
        bool init(ContextMenu*);
        void visit() override;
        void updateItem() override;

        void onAddGroup(CCObject*);
        void onAddNewGroup(CCObject*);

    public:
        static QuickGroupContextMenuItem* create(ContextMenu*);
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

class DragContextMenuItem : public PropContextMenuItem {
    public:
        using DragCallback = std::function<void(DragContextMenuItem*, float)>;
        using DragValue = std::function<float()>;

    protected:
        DragCallback m_pDragCallback = nullptr;
        DragValue m_pDragValue = nullptr;
        const char* m_sText;
        float m_fDivisor = 1.f;
        float m_fMinLimit = 0.f;
        float m_fMaxLimit = 10.f;

        bool init(
            ContextMenu*,
            const char*,
            const char*,
            float, float, float,
            DragCallback,
            DragValue
        );

        float getValue(GameObject* = nullptr) override;
        void setValue(float, bool = false) override;

        friend class ContextMenu;

    public:
        static DragContextMenuItem* create(
            ContextMenu*,
            const char* spr,
            const char* txt,
            float divisor,
            float min, float max,
            DragCallback = nullptr,
            DragValue = nullptr
        );
};

struct ContextMenuStorageItem {
    enum ItemType {
        kItemTypeKeybind,
        kItemTypeProperty,
        kItemTypeAction,
        kItemTypeQuickGroup,
        kItemTypeEmptySpacer,
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
    ContextMenuStorageItem(ItemType type, int grow = 1) {
        this->m_eType = type;
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

        enum DragItemDir {
            kDragItemDirAuto,
            kDragItemDirHorizontal,
            kDragItemDirVertical,
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
        bool m_bDrawBorder = true;
        bool m_bDisabled = false;
        bool m_bSelectObjectUnderMouse = true;
        bool m_bInvertedScrollWheel = false;
        Config m_vDefaultConfig;
        std::unordered_map<ContextType, Context> m_mContexts;
        const char* m_sFont = "Segoe UI";
        float m_fTTFFontSize = 28.f;
        float m_fFontScale = .2f;
        AnyLabelType m_eType = kAnyLabelTypeTTF;
        int m_nAnimationSpeed = 7;
        GameObject* m_pObjSelectedUnderMouse = nullptr;
        bool m_bDeselectObjUnderMouse = false;
        DragItemDir m_eDragDir = kDragItemDirAuto;
        float m_fObjectMoveGridSnap = 30.f;
        ccColor4B m_colBG = { 0, 0, 0, 200 };
        ccColor4B m_colText = { 255, 255, 255, 255 };
        ccColor4B m_colGray = { 150, 150, 150, 130 };
        ccColor4B m_colHover = { 255, 255, 255, 52 };
        ccColor4B m_colBorder = { 150, 150, 150, 20 };
        ccColor4B m_colTransparent = { 0, 0, 0, 0 };
        ccColor3B m_colSelect = { 255, 194, 90 };
        ccColor4B m_colHighlight = { 50, 255, 200, 255 };
        std::set<int> m_vQuickGroups = { 1, 2, 10, 50 };
        float m_fPaddingVertical = 10.f;
        float m_fPaddingHorizontal = 20.f;
        float m_fButtonSeparation = 2.f;

        friend class ContextMenuItem;
        friend class SpecialContextMenuItem;
        friend class KeybindContextMenuItem;
        friend class ActionContextMenuItem;
        friend class PropContextMenuItem;
        friend class QuickGroupContextMenuItem;
        friend class CustomizeCMLayer;
        friend class ContextMenuButton;

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
