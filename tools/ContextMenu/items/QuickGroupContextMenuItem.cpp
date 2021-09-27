#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool QuickGroupContextMenuItem::init(ContextMenu* menu) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_bNoHover = true;

    for (auto const& item : menu->m_vQuickGroups) {
        auto btn = ContextMenuButton::create(
            this, std::to_string(item).c_str(),
            menu_selector(QuickGroupContextMenuItem::onAddGroup)
        );
        this->addChild(btn);
    }
    this->addChild(ContextMenuButton::create(
        this, "+",
        menu_selector(QuickGroupContextMenuItem::onAddNewGroup)
    ));
    
    return true;
}

void QuickGroupContextMenuItem::visit() {
    auto count = this->getChildrenCount();

    auto size = this->getScaledContentSize();
    CCARRAY_FOREACH_B_BASE(this->getChildren(), btn, ContextMenuButton*, ix) {
        auto pos = (ix - (count - 1) / 2.f) * 15.f;
        pos += size.width / 2;
        btn->setPosition(pos, size.height / 2);
        limitNodeSize(btn, { 0, size.height - 5.f }, .5f, 0.1f);
    }

    ContextMenuItem::visit();
}

void QuickGroupContextMenuItem::onAddGroup(CCObject* pSender) {
    
}

void QuickGroupContextMenuItem::onAddNewGroup(CCObject*) {

}

bool QuickGroupContextMenuItem::mouseDownSuper(MouseButton, CCPoint const&) {
    return true;
}

void QuickGroupContextMenuItem::mouseMoveSuper(CCPoint const& pos) {
    ContextMenuItem::mouseMoveSuper(pos);
}

CCNode* QuickGroupContextMenuItem::hoversItem() {
    return nullptr;
}

QuickGroupContextMenuItem* QuickGroupContextMenuItem::create(ContextMenu* menu) {
    auto ret = new QuickGroupContextMenuItem;

    if (ret && ret->init(menu)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
