#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class EnterBPMLayer : public BrownAlertDelegate {
    protected:
        CreateGuidelinesLayer* m_pTarget;
        InputNode* m_pBPMInput;
        InputNode* m_pOffsetInput;
        InputNode* m_pPatternInput;

        void setup() override;

        void onCreate(CCObject*);

    public:
        static EnterBPMLayer* create();

        EnterBPMLayer* setTarget(CreateGuidelinesLayer* t);
};

