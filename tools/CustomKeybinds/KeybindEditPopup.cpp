#include "KeybindEditPopup.hpp"

void KeybindEditPopup::setup() {
    if (m_pStoreItem)
        m_obTypedBind = m_pStoreItem->m_obBind;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->m_bNoElasticity = true;

    auto subTitle = CCLabelBMFont::create(m_pCell->m_pBind->name.c_str(), "bigFont.fnt");
    subTitle->setColor({ 50, 155, 255 });
    subTitle->setPosition(
        winSize.width / 2,
        winSize.height / 2 + this->m_pLrSize.height / 2 - 50.0f
    );
    subTitle->setScale(.5f);
    this->m_pLayer->addChild(subTitle);

    m_pPreLabel = CCLabelBMFont::create("Press Keys...", "bigFont.fnt");
    m_pPreLabel->limitLabelWidth(this->m_pLrSize.width - 40.0f, .6f, .2f);
    m_pPreLabel->setOpacity(80);
    m_pPreLabel->setPosition(winSize / 2);
    this->m_pLayer->addChild(m_pPreLabel);

    m_pTypeLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_pTypeLabel->setPosition(winSize / 2);
    m_pTypeLabel->setVisible(false);
    this->m_pLayer->addChild(m_pTypeLabel);

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

void KeybindEditPopup::keyDown(enumKeyCodes key) {
    m_pPreLabel->setVisible(false);
    m_pTypeLabel->setVisible(true);

    m_obTypedBind = Keybind(key);

    m_pTypeLabel->setString(m_obTypedBind.toString().c_str());
    m_pTypeLabel->limitLabelWidth(this->m_pLrSize.width - 40.0f, .8f, .2f);
}

void KeybindEditPopup::onRemove(CCObject*) {
    KeybindManager::get()->removeKeybind(
        this->m_pCell->m_pItem->type,
        this->m_pCell->m_pBind,
        this->m_pStoreItem->m_obBind
    );

    this->onClose(nullptr);
}

void KeybindEditPopup::onClose(CCObject*) {
    m_pCell->updateMenu();

    BrownAlertDelegate::onClose(nullptr);
}

void KeybindEditPopup::onSet(CCObject*) {
    if (this->m_pStoreItem)
        KeybindManager::get()->editKeybind(
            this->m_pCell->m_pItem->type,
            this->m_pCell->m_pBind,
            this->m_pStoreItem->m_obBind,
            this->m_obTypedBind
        );
    else
        KeybindManager::get()->addKeybind(
            this->m_pCell->m_pItem->type,
            this->m_pCell->m_pBind,
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
        ret->init(220.0f, 160.0f, "GJ_square01.png", item ? "Edit Keybind" : "Add Keybind")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
