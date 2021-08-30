#pragma once

#include "KeybindManager.hpp"
#include "KeybindListView.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class KeybindRepeatPopup : public BrownAlertDelegate, public TextInputDelegate {
    protected:
        KeybindCell* m_pCell;

        void setup() override;
        void textChanged(CCTextInputNode*) override;
        void onEnable(CCObject*);

    public:
        static KeybindRepeatPopup* create(KeybindCell*);
};
