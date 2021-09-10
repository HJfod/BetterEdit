#pragma once

#include "../../BetterEdit.hpp"
#include "KeybindManager.hpp"
#include "KeybindingsLayer.hpp"

static constexpr const float g_fItemHeight = 27.5f;
static constexpr const BoomListType kBoomListType_Keybind
    = static_cast<BoomListType>(0x421);

ButtonSprite* createKeybindBtnSprite(const char* text, bool gold = true, const char* sprite = nullptr);

struct KeybindItem : public CCObject {
    KeybindCallback* bind;
    KeybindType type;
    KeybindingsLayerDelegate* delegate = nullptr;
    std::string text;
    bool selectMode = false;
    Keybind selectKeybind;

    inline KeybindItem(
        KeybindCallback* b,
        KeybindType t,
        KeybindingsLayerDelegate* d,
        bool select = false,
        Keybind const& kb = Keybind()
    ) {
        bind = b;
        type = t;
        text = "";
        delegate = d;
        selectMode = select;
        selectKeybind = kb;
        this->autorelease();
    }

    inline KeybindItem(
        std::string const& t,
        KeybindingsLayerDelegate* d
    ) {
        bind = nullptr;
        type = kKBEditor;
        text = t;
        delegate = d;
        this->autorelease();
    }
};

class MoreKeybindsPopup;
class KeybindEditPopup;

class KeybindCell : public TableViewCell, public FLAlertLayerProtocol {
    protected:
        KeybindCallback* m_pBind;
        KeybindItem* m_pItem;
        CCMenu* m_pMenu;

		KeybindCell(const char* name, CCSize size);

        void FLAlert_Clicked(FLAlertLayer*, bool) override;

        friend class KeybindEditPopup;
        friend class KeybindRepeatPopup;
        friend class MoreKeybindsPopup;

    public:
        void loadFromItem(KeybindItem* bind);
        void updateBGColor(int index);
        void updateMenu();
        void onEdit(CCObject*);
        void onRepeat(CCObject*);
        void onFold(CCObject*);
        void onReset(CCObject*);
        void onSelect(CCObject*);
        void onMore(CCObject*);

		static KeybindCell* create(const char* key, CCSize size);
};

class KeybindListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;

    public:
        static KeybindListView* create(CCArray* binds, float width, float height);
};
