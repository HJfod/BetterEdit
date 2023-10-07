#pragma once

#include <Geode/Geode.hpp>
#include "LockButton.hpp"

using namespace geode::prelude;

class InputScaleDelegate : public CCNode, public TextInputDelegate {
public:
    GJScaleControl* m_scaleControl;
    LockButton* m_absoluteLock;
    LockButton* m_snapLock;

    virtual void textChanged(CCTextInputNode* input) override;

    bool init(GJScaleControl* c, LockButton* absoluteLockButton, LockButton* snapLockButton) {
        m_scaleControl = c;
        m_absoluteLock = absoluteLockButton;
        m_snapLock = snapLockButton;
        return true;
    }

    static InputScaleDelegate* create(GJScaleControl* c, LockButton* absoluteLockButton, LockButton* snapLockButton) {
        auto ret = new InputScaleDelegate;
        if (ret && ret->init(c, absoluteLockButton, snapLockButton)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};