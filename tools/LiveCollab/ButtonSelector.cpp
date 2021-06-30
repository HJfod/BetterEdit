#include "ButtonSelector.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

bool ButtonSelector::init(ButtonSelector::ButtonList const& list) {
    if (!this->CCMenu::init())
        return false;
    
    this->m_pButtons = CCArray::create();
    this->m_pButtons->retain();
    
    for (auto btxt : list) {
        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(btxt.name, 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f),
            this,
            (SEL_MenuHandler)&ButtonSelector::onSelect
        );
        
        btn->setUserObject(btxt.arr);
        this->m_pButtons->addObject(btn);

        this->addChild(btn);
    }

    this->alignItemsHorizontallyWithPadding(5.0f);

    if (this->m_pButtons->count())
        this->onSelect(this->m_pButtons->objectAtIndex(0u));

    return true;
}

void ButtonSelector::onSelect(CCObject* pSender) {
    CCARRAY_FOREACH_B_TYPE(this->m_pButtons, btn, CCMenuItemSpriteExtra) {
        CCARRAY_FOREACH_B_TYPE(as<CCArray*>(btn->getUserObject()), node, CCNode)
            node->setVisible(btn == pSender);
    
        as<ButtonSprite*>(btn->getNormalImage())
            ->updateBGImage(btn == pSender ? "GJ_button_02.png" : "GJ_button_01.png");
    }
}

ButtonSelector* ButtonSelector::create(ButtonSelector::ButtonList const& list) {
    auto pRet = new ButtonSelector();

    if (pRet && pRet->init(list)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

ButtonSelector::~ButtonSelector() {
    CCMenu::~CCMenu();
}
