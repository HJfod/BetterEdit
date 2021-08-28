#include "KeybindEditPopup.hpp"

void KeybindEditPopup::setup() {
    if (m_pStoreItem)
        m_obTypedBind = m_pStoreItem->m_obBind;
    
    this->m_bNoElasticity = true;

    auto setBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor<ButtonSprite*>()
            .fromButtonSprite(
                m_pStoreItem ? "Set" : "Add", "GJ_button_01.png", "bigFont.fnt"
            )
            .scale(.6f)
            .done(),
        this,
        menu_selector(KeybindEditPopup::onSet)
    );
    this->m_pButtonMenu->addChild(setBtn);

    CCMenuItemSpriteExtra* removeBtn = nullptr;
    if (m_pStoreItem) {
        removeBtn = CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromButtonSprite(
                    "Remove", "GJ_button_06.png", "bigFont.fnt"
                )
                .scale(.6f)
                .done(),
            this,
            menu_selector(KeybindEditPopup::onRemove)
        );
        this->m_pButtonMenu->addChild(removeBtn);
    }

    this->m_pButtonMenu->alignItemsHorizontallyWithPadding(5.0f);

    setBtn->setPositionY(-this->m_pLrSize.height / 2 + 30.0f);
    if (removeBtn) removeBtn->setPositionY(-this->m_pLrSize.height / 2 + 30.0f);
}

void KeybindEditPopup::onRemove(CCObject*) {
    KeybindManager::get()->removeKeybind(
        this->m_pCell->m_pItem->type,
        this->m_pCell->m_pBind,
        this->m_pStoreItem->m_obBind
    );

    this->onClose(nullptr);
}

void KeybindEditPopup::onSet(CCObject*) {
    KeybindManager::get()->editKeybind(
        this->m_pCell->m_pItem->type,
        this->m_pCell->m_pBind,
        this->m_pStoreItem->m_obBind,
        this->m_obTypedBind
    );

    this->onClose(nullptr);
}

KeybindEditPopup* KeybindEditPopup::create(KeybindCell* cell, KeybindStoreItem* item) {
    auto ret = new KeybindEditPopup;

    if (
        ret &&
        ((ret->m_pCell = cell) || true) &&
        ((ret->m_pStoreItem = item) || true) &&
        ret->init(250.0f, 220.0f, "GJ_square01.png", item ? "Edit Keybind" : "Add Keybind")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
