#include "VisibilityToggle.hpp"

void VisibilityToggle::onToggle(CCObject*) {
    if (this->m_pSetter)
        this->m_pSetter(!this->isToggled(), this);
}

void VisibilityToggle::updateState() {
    if (this->m_pGetter)
        this->toggle(this->m_pGetter());
}

VisibilityToggle* VisibilityToggle::invokeSetter() {
    if (this->m_pSetter)
        this->m_pSetter(this->m_pGetter(), this);

    return this;
}

VisibilityToggle* VisibilityToggle::create(
    const char* sprite,
    VisibilityToggle::Getter getter,
    VisibilityToggle::Setter setter
) {
    auto ret = new VisibilityToggle;

    if (ret && ret->init(
        createEditBtnSprite(sprite, false),
        createEditBtnSprite(sprite, true),
        ret,
        (SEL_MenuHandler)&VisibilityToggle::onToggle
    )) {
        ret->m_pSetter = setter;
        ret->m_pGetter = getter;
        //ret->updateState();
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

VisibilityToggle* VisibilityToggle::create(
    const char* sprite
) {
    auto ret = new VisibilityToggle;

    if (ret && ret->init(
        createEditBtnSprite(sprite, false),
        createEditBtnSprite(sprite, true),
        ret,
        nullptr
    )) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}