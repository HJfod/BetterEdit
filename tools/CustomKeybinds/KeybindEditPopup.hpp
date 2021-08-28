#pragma once

#include "KeybindManager.hpp"
#include "KeybindListView.hpp"
#include <BrownAlertDelegate.hpp>

struct KeybindStoreItem : public CCObject {
    Keybind m_obBind;

    inline KeybindStoreItem(Keybind const& bind) {
        m_obBind = bind;
        this->autorelease();
    }
};

class KeybindEditPopup : public BrownAlertDelegate {
    protected:
        KeybindCell* m_pCell;
        KeybindStoreItem* m_pStoreItem;
        Keybind m_obTypedBind;

        void setup() override;
        void onRemove(CCObject*);
        void onSet(CCObject*);

    public:
        static KeybindEditPopup* create(KeybindCell*, KeybindStoreItem*);
};
