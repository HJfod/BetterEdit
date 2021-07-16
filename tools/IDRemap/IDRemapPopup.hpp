#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class IDRemapPopup : public BrownAlertDelegate {
    protected:
        InputNode* m_pStartInput;
        InputNode* m_pEndInput;
        InputNode* m_pOffsetInput;

        void setup() override;

        void onRemap(CCObject*);
    
    public:
        static IDRemapPopup* create();
};
