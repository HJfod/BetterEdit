#pragma once

#include "../../BetterEdit.hpp"
#include "KeybindManager.hpp"
#include "KeybindingsLayer.hpp"

static constexpr const float g_fItemHeight = 27.5f;
static constexpr const BoomListType kBoomListType_Keybind
    = static_cast<BoomListType>(0x421);

struct KeybindItem : public CCObject {
    KeybindCallback* bind;
    KeybindType type;
    KeybindingsLayerDelegate* delegate = nullptr;
    const char* text;

    inline KeybindItem(
        KeybindCallback* b,
        KeybindType t,
        KeybindingsLayerDelegate* d
    ) {
        bind = b;
        type = t;
        text = nullptr;
        delegate = d;
        this->autorelease();
    }

    inline KeybindItem(
        const char* t,
        KeybindingsLayerDelegate* d
    ) {
        bind = nullptr;
        type = kKBEditor;
        text = t;
        delegate = d;
        this->autorelease();
    }
};

class KeybindEditPopup;

class KeybindCell : public TableViewCell, public FLAlertLayerProtocol {
    protected:
        KeybindCallback* m_pBind;
        KeybindItem* m_pItem;
        CCMenu* m_pMenu;

		KeybindCell(const char* name, CCSize size);

        void FLAlert_Clicked(FLAlertLayer*, bool) override;

        friend class KeybindEditPopup;

    public:
        void loadFromItem(KeybindItem* bind);
        void updateBGColor(int index);
        void updateMenu();
        void onEdit(CCObject*);
        void onFold(CCObject*);
        void onReset(CCObject*);

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
