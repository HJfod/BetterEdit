#pragma once

#include "../../BetterEdit.hpp"

enum ThreeWayBool : char {
    kThreeWayBoolEither = 0,
    kThreeWayBoolFalse,
    kThreeWayBoolTrue,
};

class ThreeWayBoolSelect;

typedef void (CCObject::*SEL_ThreeWayBool)(ThreeWayBoolSelect*, ThreeWayBool);
#define threeway_selector(_SELECTOR) (SEL_ThreeWayBool)(&_SELECTOR)

class ThreeWayBoolSelect : public CCMenu {
    protected:
        CCLabelBMFont* m_pLabel;
        ThreeWayBool m_eState;
        SEL_ThreeWayBool m_callback = nullptr;
        CCObject* m_pTarget = nullptr;
        CCMenuItemToggler* m_pFalseBtn;
        CCMenuItemToggler* m_pEitherBtn;
        CCMenuItemToggler* m_pTrueBtn;

        bool init(const char*, CCObject*, SEL_ThreeWayBool);

        void onSelect(CCObject*);
    
    public:
        static ThreeWayBoolSelect* create(const char* text, CCObject*, SEL_ThreeWayBool);

        void setValue(ThreeWayBool);
};
