#pragma once

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include "Editor.hpp"

class BEMenuItemToggler : public CCMenuItemSpriteExtra {
public:
    std::function<bool()> m_getter;
    std::function<void(bool)> m_setter;
    std::function<bool()> m_shouldEnable;
    CCNode* m_offNode = nullptr;
    CCNode* m_onNode = nullptr;
    bool m_toggled = false;

    static BEMenuItemToggler* create(CCNode* offNode, CCNode* onNode, auto getter, auto setter, auto shouldEnable) {
        auto ret = new BEMenuItemToggler();
        if (ret && ret->init(offNode, onNode, getter, setter, shouldEnable)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(CCNode* offNode, CCNode* onNode, auto getter, auto setter, auto shouldEnable) {
        if (!CCMenuItemSpriteExtra::init(offNode, nullptr, nullptr, nullptr)) {
            return false;
        }

        m_offNode = offNode;
        m_offNode->retain();
        m_onNode = onNode;
        m_onNode->retain();
        m_getter = getter;
        m_setter = setter;
        m_shouldEnable = shouldEnable;

        this->updateState();

        return true;
    }

    void updateState() {
        this->toggle(m_getter());
        if (m_shouldEnable) be::enableButton(this, m_shouldEnable());
    }
    void toggle(bool toggled) {
        m_toggled = toggled;
        this->setNormalImage(toggled ? m_onNode : m_offNode);
        this->updateSprite();
    }
    bool isToggled() const {
        return m_toggled;
    }

    void activate() override {
        toggle(!m_toggled);
        m_setter(m_toggled);
        CCMenuItemSpriteExtra::activate();
    }

    ~BEMenuItemToggler() {
        CC_SAFE_RELEASE(m_offNode);
        CC_SAFE_RELEASE(m_onNode);
    }
};
