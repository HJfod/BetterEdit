#include "../ContextMenu.hpp"

bool ContextMenuItemWithBGAndPossiblyText::init(AnyLabel* label) {
    if (!CCNode::init())
        return false;
    
    this->m_pLabel = label;
    if (label) {
        this->addChild(label);
    }

    return true;
}

void ContextMenuItemWithBGAndPossiblyText::draw() {
    auto size = this->getScaledContentSize();
    if (this->m_pLabel) {
        auto csize = this->m_pLabel->getScaledContentSize() * 1.1f;
        if (csize.width < 25.f)
            csize.width = 25.f;
        this->setContentSize(csize);
        size = this->getScaledContentSize();
        this->m_pLabel->setPosition(
            size / 2
        );
    }
    ccDrawSolidRect(
        { 0, 0 }, size,
        this->m_obColor
    );
    CCNode::draw();
}

void ContextMenuItemWithBGAndPossiblyText::setColor(ccColor4B col) {
    this->m_obColor = to4f(col);
}

void ContextMenuItemWithBGAndPossiblyText::setString(const char* str) {
    if (this->m_pLabel)
        this->m_pLabel->setString(str);
}

const char* ContextMenuItemWithBGAndPossiblyText::getString() const {
    if (this->m_pLabel)
        return this->m_pLabel->getString();
    return "";
}

ContextMenuItemWithBGAndPossiblyText* ContextMenuItemWithBGAndPossiblyText::create(
    AnyLabel* label
) {
    auto ret = new ContextMenuItemWithBGAndPossiblyText;

    if (ret && ret->init(label)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
