#pragma once

#include "KeybindManager.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class KeybindSettingsLayer :
    public BrownAlertDelegate,
    public TextInputDelegate
{
    protected:
        void setup() override;
        void textChanged(CCTextInputNode*) override;
        void onEnable(CCObject*);
        void onClose(CCObject*) override;

    public:
        static KeybindSettingsLayer* create();
};
