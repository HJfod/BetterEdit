#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class InputScaleDelegate : public CCNode, public TextInputDelegate {
public:
    GJScaleControl* m_scaleControl;

    virtual void textChanged(CCTextInputNode* input) override;

    bool init(GJScaleControl* c) {
        this->m_scaleControl = c;
        return true;
    }

    static InputScaleDelegate* create(GJScaleControl* c) {
        auto ret = new InputScaleDelegate;
        if (ret && ret->init(c)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};