#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool QuickGroupContextMenuItem::init(ContextMenu* menu) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_bNoHover = true;

    for (auto const& item : menu->m_vQuickGroups) {
        auto btn = ContextMenuButton::createToggle(
            this, std::to_string(item).c_str(),
            menu_selector(QuickGroupContextMenuItem::onAddGroup)
        );
        btn->setTag(item);
        this->addChild(btn);
    }
    this->addChild(ContextMenuButton::create(
        this, "+",
        menu_selector(QuickGroupContextMenuItem::onAddNewGroup)
    ));
    
    return true;
}

void QuickGroupContextMenuItem::visit() {
    ContextMenuItem::visit();
}

void QuickGroupContextMenuItem::updateItem() {
    std::set<short> ids;
    CCARRAY_FOREACH_B_TYPE(EditorUI::get()->getSelectedObjects(), obj, GameObject) {
        for (int i = 0; i < obj->m_nGroupCount; i++) {
            ids.insert(obj->m_pGroups[i]);
        }
    }

    auto count = this->getChildrenCount();

    auto size = this->getContentSize();
    
    float width = 0.f;
    CCARRAY_FOREACH_B_BASE(this->getChildren(), btn, ContextMenuButton*, ix) {
        limitNodeSize(btn, { 0, size.height - 5.f }, .5f, 0.1f);
        width += btn->getScaledContentSize().width + m_pMenu->m_fButtonSeparation;
    }

    float pos = .0f;
    CCARRAY_FOREACH_B_BASE(this->getChildren(), btn, ContextMenuButton*, ix) {
        pos += btn->getScaledContentSize().width / 2;

        btn->setPosition(pos + size.width / 2 - width / 2, size.height / 2);
        btn->toggle(ids.count(btn->getTag()));

        pos += btn->getScaledContentSize().width / 2 + m_pMenu->m_fButtonSeparation;
    }
}

void QuickGroupContextMenuItem::onAddGroup(CCObject* pSender) {
    CCARRAY_FOREACH_B_TYPE(EditorUI::get()->getSelectedObjects(), obj, GameObject) {
        if (as<ContextMenuButton*>(pSender)->isToggled()) {
            obj->addToGroup(pSender->getTag());
        } else {
            obj->removeFromGroup(pSender->getTag());
        }
    }
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
