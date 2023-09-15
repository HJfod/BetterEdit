#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/CreateGuidelinesLayer.hpp>

using namespace geode::prelude;

class EnterBPMLayer : public Popup<> {
    protected:
        CreateGuidelinesLayer* m_target;
        CCTextInputNode* m_BPMInput;
        CCTextInputNode* m_offsetInput;
        CCTextInputNode* m_patternInput;

        bool setup() override;

        void onCreate(CCObject*);

    public:
        static EnterBPMLayer* create();

        EnterBPMLayer* setTarget(CreateGuidelinesLayer* cgl);
};