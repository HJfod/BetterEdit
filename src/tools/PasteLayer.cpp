#include "PasteLayer.hpp"
#include "../layers/EditorUI.hpp"
#include <algorithm>

void PasteLayer::onPaste(cocos2d::CCObject*) {
    this->onClose(nullptr);

    std::string s = this->m_pInput->getString();

    std::replace(s.begin(), s.end(), '\n', ';');

    auto obj = EditorUI::pasteObjectsFromString(EditorUI::sharedUI(), s);

    for (unsigned int i = 0; i < obj->count(); i++)
        EditorUI::sharedUI()->deselect(
            reinterpret_cast<gd::GameObject*>(
                obj->objectAtIndex(i)
            )
        );
}

void PasteLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    this->m_pInput = InputNode::create(
        250.0f,
        "[Object string]",
        "chatFont.fnt",
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.;:|-_=½(){}[]%&#+?\n\r",
        150
    );
    this->m_pInput->setPosition(winSize / 2);
    this->m_pLayer->addChild(this->m_pInput);

    auto textArea = gd::TextArea::create(
        "goldFont.fnt",
        cocos2d::CCSprite::create(),
        "Yo wtf please work",
        440.0f,
        28.0f,
        .75f
    );
    this->m_pButtonMenu->addChild(textArea);

    auto applyButton = gd::CCMenuItemSpriteExtra::create(
        gd::ButtonSprite::create(
            "Paste", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .8f
        ),
        this,
        (cocos2d::SEL_MenuHandler)(&PasteLayer::onPaste)
    );
    applyButton->setPositionY(- this->m_pLrSize.height / 2 + 30.0f);

    this->m_pButtonMenu->addChild(applyButton);
}

PasteLayer* PasteLayer::create() {
    auto pRet = new PasteLayer();

    if (pRet && pRet->init(300.0f, 200.0f, "GJ_square01.png", "Paste Objects From String")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
