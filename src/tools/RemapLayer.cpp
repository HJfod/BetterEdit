#include "RemapLayer.hpp"
#include "../nodes/InputNode.hpp"

bool RemapItem::init(float _width) {
    auto bg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png",
        { 0, 0, 80.0f, 80.0f }
    );
    bg->setColor({ 107, 58, 36 });
    bg->setContentSize({ 340.0f, 50.0f });
    this->addChild(bg);

    auto fromInput = InputNode::create(80.0f, "Start", "0123456789", 4);
    fromInput->setPositionX(-120.0f);
    this->addChild(fromInput);

    auto toInput = InputNode::create(80.0f, "End", "0123456789", 4);
    toInput->setPositionX(0.0f);
    this->addChild(toInput);

    auto offInput = InputNode::create(80.0f, "To", "0123456789", 4);
    offInput->setPositionX(120.0f);
    this->addChild(offInput);

    auto textBetween = cocos2d::CCLabelBMFont::create("-", "bigFont.fnt");
    textBetween->setPositionX(-60.f);
    this->addChild(textBetween);

    auto textArrow = cocos2d::CCLabelBMFont::create("->", "bigFont.fnt");
    textArrow->setPositionX(60.f);
    this->addChild(textArrow);

    this->setScale(.5f);

    return true;
}

RemapItem* RemapItem::create(float _w) {
    auto pRet = new RemapItem();

    if (pRet && pRet->init(_w)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void RemapLayer::onAdd(cocos2d::CCObject*) {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto obj = RemapItem::create(this->m_pLrSize.width - 80.0f);

    obj->setPosition(
        0,
        40.0f - 30.0f * this->m_pRemapItems->getChildrenCount()
    );

    this->m_pRemapItems->addChild(obj);
}

void RemapLayer::onApply(cocos2d::CCObject*) {
}

void RemapLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    auto addButton = gd::CCMenuItemSpriteExtra::create(
        gd::ButtonSprite::create(
            "Add Remap", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .8f
        ),
        this,
        (cocos2d::SEL_MenuHandler)(&RemapLayer::onAdd)
    );
    addButton->setPositionY(this->m_pLrSize.height / 2 - 60.0f);
    this->m_pButtonMenu->addChild(addButton);

    this->m_pRemapItems = cocos2d::CCLayer::create();
    this->m_pRemapItems->setPosition(winSize / 2);
    this->m_pLayer->addChild(this->m_pRemapItems);

    auto applyButton = gd::CCMenuItemSpriteExtra::create(
        gd::ButtonSprite::create(
            "Apply", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .8f
        ),
        this,
        (cocos2d::SEL_MenuHandler)(&RemapLayer::onApply)
    );
    applyButton->setPositionY(- this->m_pLrSize.height / 2 + 30.0f);
    this->m_pButtonMenu->addChild(applyButton);
}

RemapLayer* RemapLayer::create() {
    auto pRet = new RemapLayer();

    if (pRet && pRet->init(350.0f, 280.0f, "GJ_square01.png", "Remap Group IDs")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

RemapLayer::~RemapLayer() {
    this->m_pRemapItems->release();
}

