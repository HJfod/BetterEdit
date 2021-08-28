#pragma once

#include "../../BetterEdit.hpp"
#include "KeybindManager.hpp"

static constexpr const float g_fItemHeight = 27.5f;
static constexpr const BoomListType kBoomListType_Keybind
    = static_cast<BoomListType>(0x421);

struct KeybindItem : public CCObject {
    KeybindCallback* bind;
    KeybindType type;
    const char* text;

    inline KeybindItem(KeybindCallback* b, KeybindType t) {
        bind = b;
        type = t;
        text = nullptr;
        this->autorelease();
    }

    inline KeybindItem(const char* t) {
        bind = nullptr;
        type = kKBEditor;
        text = t;
        this->autorelease();
    }
};

class KeybindEditPopup;

class KeybindCell : public TableViewCell {
    protected:
        KeybindCallback* m_pBind;
        KeybindItem* m_pItem;
        CCMenu* m_pMenu;

		KeybindCell(const char* name, CCSize size);

        friend class KeybindEditPopup;

    public:
        void loadFromItem(KeybindItem* bind);
        void updateBGColor(int index);
        void updateMenu();
        void onEdit(CCObject*);

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
