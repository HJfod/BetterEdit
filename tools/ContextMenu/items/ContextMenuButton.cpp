#include "../ContextMenu.hpp"

bool ContextMenuButton::init(
    ContextMenuItem* item, const char* txt, SEL_MenuHandler cb
) {
    this->m_pBG = ContextMenuItemWithBGAndPossiblyText::create( 
        item->createLabel(txt)
    );
    if (!CCMenuItemSprite::initWithNormalSprite(
        this->m_pBG, nullptr, nullptr,
        item, cb
    )) return false;
    if (!CCMenuItemSpriteExtra::init(this->m_pBG))
        return false;
    
    this->m_pItem = item;
    this->m_pBG->setColor(this->m_pItem->m_pMenu->m_colGray);

    return true;
}

bool ContextMenuButton::mouseDownSuper(MouseButton btn, CCPoint const&) {
    this->selected();
    return true;
}

bool ContextMenuButton::mouseUpSuper(MouseButton btn, CCPoint const&) {
    this->activate();
    return true;
}

void ContextMenuButton::mouseLeaveSuper(CCPoint const&) {
    this->unselected();
}

void ContextMenuButton::visit() {
    if (this->m_bSuperMouseDown) {
        this->m_pBG->setColor(this->m_pItem->m_pMenu->m_colHighlight);
    } else {
        this->m_pBG->setColor(
            this->m_bSuperMouseHovered ?
                this->m_pItem->m_pMenu->m_colHover :
                this->m_pItem->m_pMenu->m_colGray
        );
    }

    this->setContentSize(this->m_pBG->getScaledContentSize());

    CCMenuItemSpriteExtra::visit();
}

ContextMenuButton* ContextMenuButton::create(
    ContextMenuItem* item, const char* txt, SEL_MenuHandler cb
) {
    auto ret = new ContextMenuButton;

    if (ret && ret->init(item, txt, cb)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}