#include "OffsetNextFreePopup.hpp"

bool OffsetNextFreePopup::setup(OffsetType offsetType, FLAlertLayer* parent) {
    this->setTitle("Setup Offset");
    m_offsetType = offsetType;
    m_parent = parent;
    auto pos = CCDirector::sharedDirector()->getWinSize() / 2;
    size_t offset = 0;

    switch (offsetType) {
        case OffsetType::Color:
            offset = OffsetManager::get()->m_colorOffset;
            break;
        case OffsetType::Group:
            offset = OffsetManager::get()->m_groupOffset;
            break;
    }

    auto bg = CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setScale(0.5f);
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(100);
    bg->setContentSize({ 115.0f, 75.0f });
    bg->setPosition(pos);
    bg->setID("offset-input-bg"_spr);
    m_mainLayer->addChild(bg);

    auto input = CCTextInputNode::create(60.0f, 30.0f, "0", "bigFont.fnt");
    input->setPosition(pos);
    input->setLabelPlaceholderColor({ 120, 120, 120 });
    input->setAllowedChars("0123456789");
    input->m_maxLabelLength = 4;
    input->setScale(0.7f);
    input->setID("offset-input"_spr);
    if (offset != 0) {
        input->setString(std::to_string(offset).c_str());
    }
    m_offsetInput = input;
    m_mainLayer->addChild(input);

    auto center = CCDirector::sharedDirector()->getWinSize() / 2;

    auto menu = CCMenu::create();
    menu->setID("offset-menu"_spr);
    auto spr = ButtonSprite::create("Apply", 50, true, "goldFont.fnt", "GJ_button_01.png", 30, 1.f);
    auto okButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(OffsetNextFreePopup::onClose)
    );
    okButton->setID("ok-button"_spr);
    okButton->setPositionY(-50.f);
    menu->addChild(okButton);

    if (offset != 0) {
        auto spr2 = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        spr2->setScale(0.6f);
        auto clearButton = CCMenuItemSpriteExtra::create(
            spr2,
            this,
            menu_selector(OffsetNextFreePopup::onClear)
        );
        clearButton->setID("clear-button"_spr);
        clearButton->setPosition(-65.f, -50.f);
        menu->addChild(clearButton);
    }

    m_mainLayer->addChild(menu);
    return true;
}

void OffsetNextFreePopup::onClose(CCObject* sender) {
    this->updateOffset();
    this->updateParentButton();
    switch (m_offsetType) {
        case OffsetType::Color: {
            auto parent = static_cast<CustomizeObjectLayer*>(m_parent);
            parent->onNextColorChannel(sender);
        } break;
        case OffsetType::Group: {
            auto parent = static_cast<SetGroupIDLayer*>(m_parent);
            parent->onNextGroupID1(sender);
        } break;
    }
    Popup::onClose(sender);
}

size_t OffsetNextFreePopup::getOffsetFromInput() {
    size_t offset = 0;
    std::string strValue = m_offsetInput->getString();
    if (strValue.length() != 0) {
        try {
            offset = std::stoi(strValue);
        } catch (...) {}
    }
    if (offset > 999) {
        offset = 999;
    }

    return offset;
}

void OffsetNextFreePopup::updateOffset() {
    auto offset = this->getOffsetFromInput();
    switch (m_offsetType) {
        case OffsetType::Color:
            OffsetManager::get()->m_colorOffset = offset;
            break;
        case OffsetType::Group:
            OffsetManager::get()->m_groupOffset = offset;
            break;
    }
}

void OffsetNextFreePopup::updateParentButton() {
    size_t offset;
    switch (m_offsetType) {
        case OffsetType::Color:
            offset = OffsetManager::get()->m_colorOffset;
            break;
        case OffsetType::Group:
            offset = OffsetManager::get()->m_groupOffset;
            break;
    }
    std::string sprite;
    if (offset == 0) {
        sprite = "GJ_button_04.png";
    } else {
        sprite = "GJ_button_02.png";
    }

    auto spr = ButtonSprite::create(std::to_string(offset).c_str(), 20, true, "goldFont.fnt", sprite.c_str(), 25, 0.6f);
    CCMenu* menu;
    menu = static_cast<CCMenu*>(m_parent->m_mainLayer->getChildByID("next-free-menu"));
    if (!menu) {
        menu = getChildOfType<CCMenu>(m_parent->m_mainLayer, 0);
    }
    auto button = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("offset-button"_spr));
    button->setNormalImage(spr);
}

void OffsetNextFreePopup::onClear(CCObject* sender) {
    m_offsetInput->setString("0");

    this->onClose(sender);
}