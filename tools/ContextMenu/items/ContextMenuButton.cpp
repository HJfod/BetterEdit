#include "../ContextMenu.hpp"

bool ContextMenuButton::init(
    ContextMenuItem* item, const char* txt, SEL_MenuHandler cb, bool toggle
) {
    this->m_pBG = ContextMenuItemWithBGAndPossiblyText::create( 
        item->createLabel(txt),
        item->m_pMenu->m_fPaddingHorizontal,
        item->m_pMenu->m_fPaddingVertical
    );
    if (!CCMenuItemSprite::initWithNormalSprite(
        this->m_pBG, nullptr, nullptr,
        item, cb
    )) return false;
    if (!CCMenuItemSpriteExtra::init(this->m_pBG))
        return false;
    
    this->m_bToggleable = toggle;
    this->m_pItem = item;
    this->m_pBG->setColor(this->m_pItem->m_pMenu->m_colGray);

    return true;
}

bool ContextMenuButton::mouseDownSuper(MouseButton btn, CCPoint const&) {
    if (btn == kMouseButtonLeft) {
        this->selected();
    }
    return true;
}

bool ContextMenuButton::mouseUpSuper(MouseButton btn, CCPoint const&) {
    if (btn == kMouseButtonLeft) {
        if (this->m_bToggleable)
            this->m_bToggled = !this->m_bToggled;
        this->activate();
    }
    return true;
}

void ContextMenuButton::mouseLeaveSuper(CCPoint const&) {
    this->unselected();
}

void ContextMenuButton::mouseEnterSuper(CCPoint const&) {
    if (this->m_bSuperMouseDown) {
        this->selected();
    }
}

void ContextMenuButton::visit() {
    if (this->m_bSuperMouseDown || (this->m_bToggleable && this->m_bToggled)) {
        auto h = this->m_pItem->m_pMenu->m_colHighlight;
        if (h.a > 180) h.a = 180;
        if (!this->m_bSuperMouseHovered && h.a > 120) h.a = 120;
        this->m_pBG->setColor(h);
    } else {
        this->m_pBG->setColor(
            this->m_bSuperMouseHovered ?
                this->m_pItem->m_pMenu->m_colHover :
                this->m_pItem->m_pMenu->m_colGray
        );
    }

    this->setContentSize(this->m_pBG->getScaledContentSize());
    this->setSuperMouseHitSize(this->getScaledContentSize() * 2.f);
    this->setSuperMouseHitOffset({ 0, 0 });

    CCMenuItemSpriteExtra::visit();
}

void ContextMenuButton::toggle(bool b) {
    this->m_bToggled = b;
}

bool ContextMenuButton::isToggled() const {
    return m_bToggleable && m_bToggled;
}

ContextMenuButton* ContextMenuButton::create(
    ContextMenuItem* item, const char* txt, SEL_MenuHandler cb
) {
    auto ret = new ContextMenuButton;

    if (ret && ret->init(item, txt, cb, false)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

ContextMenuButton* ContextMenuButton::createToggle(
    ContextMenuItem* item, const char* txt, SEL_MenuHandler cb
) {
    auto ret = new ContextMenuButton;

    if (ret && ret->init(item, txt, cb, true)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
