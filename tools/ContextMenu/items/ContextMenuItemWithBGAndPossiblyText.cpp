#include "../ContextMenu.hpp"

bool ContextMenuItemWithBGAndPossiblyText::init(
    AnyLabel* label, float padh, float padv
) {
    if (!CCNode::init())
        return false;
    
    this->m_fPaddingHorizontal = padh;
    this->m_fPaddingHorizontal = padv;
    this->m_pLabel = label;
    if (label) {
        this->addChild(label);
        this->updateSize();
    }

    return true;
}

void ContextMenuItemWithBGAndPossiblyText::updateSize() {
    if (this->m_pLabel) {
        auto size = this->m_pLabel->getScaledContentSize();
        this->setContentSize({
            size.width + m_fPaddingHorizontal * 2.f,
            size.height + m_fPaddingVertical * 2.f
        });
        this->m_pLabel->setPosition(this->getContentSize() / 2);
    }
}

void ContextMenuItemWithBGAndPossiblyText::draw() {
    ccDrawSolidRect(
        { 0, 0 }, this->getScaledContentSize(),
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
    AnyLabel* label, float padh, float padv
) {
    auto ret = new ContextMenuItemWithBGAndPossiblyText;

    if (ret && ret->init(label, padv, padh)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
