#include "MoreKeybindsPopup.hpp"
#include "KeybindEditPopup.hpp"

void MoreKeybindsPopup::setup() {
    auto binds = KeybindManager::get()->getKeybindsForCallback(
        m_pCell->m_pItem->type, m_pCell->m_pBind
    );

    auto menu = CCMenu::create();

    for (auto const& bind : binds) {
        auto spr = createKeybindBtnSprite(bind.toString().c_str());
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this->m_pCell, menu_selector(KeybindCell::onEdit)
        );
        auto width = spr->getScaledContentSize().width;
        if (width + 10.0f > this->m_obSize.width) {
            this->m_obSize.width = width + 10.0f;
            this->m_pBG->setContentSize(this->m_obSize * 2);
        }
        btn->setUserObject(new KeybindStoreItem(bind));
        menu->addChild(btn);
    }

    auto spr = createKeybindBtnSprite("+");
    auto btn = CCMenuItemSpriteExtra::create(
        spr, this->m_pCell, menu_selector(KeybindCell::onEdit)
    );
    btn->setUserObject(nullptr);
    menu->addChild(btn);

    menu->alignItemsVerticallyWithPadding(2.0f);
    menu->setPosition(0, 0);

    this->addChild(menu);
}

MoreKeybindsPopup* MoreKeybindsPopup::create(KeybindCell* cell, CCMenuItemSpriteExtra* btn) {
    auto ret = new MoreKeybindsPopup;

    if (
        ret &&
        (ret->m_pCell = cell) &&
        ret->init(
        btn->getParent()->convertToWorldSpace(btn->getPosition()),
        { 150.f, (KeybindManager::get()->getKeybindsForCallback(
            cell->m_pItem->type, cell->m_pBind
        ).size() + 1) * 30.0f },
        kContextPopupDirectionDown,
        kContextPopupTypeBrown
    )) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
