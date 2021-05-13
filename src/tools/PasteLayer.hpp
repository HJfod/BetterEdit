#pragma once

#include "../nodes/BrownAlertDelegate.hpp"
#include "../nodes/InputNode.hpp"

class PasteLayer : public BrownAlertDelegate {
    protected:
        virtual void setup() override;

        InputNode* m_pInput;

        void onPaste(cocos2d::CCObject*);

    public:
        static PasteLayer* create();
};
