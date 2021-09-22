#pragma once

#include "KeybindManager.hpp"
#include "KeybindListView.hpp"
#include <BrownAlertDelegate.hpp>
#include "SuperKeyboardManager.hpp"
#include "SuperMouseManager.hpp"

struct KeybindStoreItem : public CCObject {
    Keybind m_obBind;

    inline KeybindStoreItem(Keybind const& bind) {
        m_obBind = bind;
        this->autorelease();
    }
};

class KeybindEditPopup :
    public BrownAlertDelegate,
    public SuperKeyboardDelegate,
    public SuperMouseDelegate
{
    protected:
        KeybindCell* m_pCell;
        KeybindStoreItem* m_pStoreItem;
        Keybind m_obTypedBind;
        CCLabelBMFont* m_pTypeLabel;
        CCLabelBMFont* m_pPreLabel;
        CCLabelBMFont* m_pInfoLabel;

        void setup() override;
        void onRepeat(CCObject*);
        void onRemove(CCObject*);
        void onSet(CCObject*);
        void onClose(CCObject*) override;
        void keyDown(enumKeyCodes) override;
        bool keyDownSuper(enumKeyCodes) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        void updateLabel();

    public:
        static KeybindEditPopup* create(KeybindCell*, KeybindStoreItem*);
};
