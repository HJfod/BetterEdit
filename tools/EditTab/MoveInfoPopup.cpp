#include "MoveInfoPopup.hpp"

CCMenuItemSpriteExtra* createBtn(
    const char* spr,
    const char* sizeTxt,
    float scale
) {
    auto btn = LevelEditorLayer::get()->m_pEditorUI->getSpriteButton(
        spr, nullptr, nullptr, .9f
    );

    if (sizeTxt) {
        auto label = CCLabelBMFont::create(sizeTxt, "bigFont.fnt");
        label->setScale(.35f);
        label->setZOrder(50);
        label->setPosition(btn->getContentSize().width / 2, 11.0f);
        btn->addChild(label);
    }

    as<ButtonSprite*>(btn->getNormalImage())
        ->m_pSubSprite->setScale(scale);

    btn->getNormalImage()->setScale(.583f);

    return btn;
}

void MoveInfoPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn0 = createBtn("edit_rightBtn_001.png", nullptr, 1.f);
    btn0->setPosition(winSize.width / 2 - 60.f, winSize.height / 2 - 30.f);
    this->m_pLayer->addChild(btn0);
    
    auto btn1 = createBtn("edit_rightBtn2_001.png", nullptr, 1.f);
    btn1->setPosition(winSize.width / 2 - 20.f, winSize.height / 2 - 30.f);
    this->m_pLayer->addChild(btn1);
    
    auto btn2 = createBtn("edit_rightBtn3_001.png", nullptr, 1.f);
    btn2->setPosition(winSize.width / 2 + 20.f, winSize.height / 2 - 30.f);
    this->m_pLayer->addChild(btn2);
    
    auto btn3 = createBtn("edit_rightBtn_001.png", nullptr, .8f);
    btn3->setPosition(winSize.width / 2 + 60.f, winSize.height / 2 - 30.f);
    this->m_pLayer->addChild(btn3);
    
    auto btn4 = createBtn("edit_rightBtn2_001.png", "1/2", 1.f);
    btn4->setPosition(winSize.width / 2 - 60.f, winSize.height / 2 + 30.f);
    this->m_pLayer->addChild(btn4);
    
    auto btn5 = createBtn("edit_rightBtn2_001.png", "1/4", 1.f);
    btn5->setPosition(winSize.width / 2 - 20.f, winSize.height / 2 + 30.f);
    this->m_pLayer->addChild(btn5);
    
    auto btn6 = createBtn("edit_rightBtn2_001.png", "1/8", 1.f);
    btn6->setPosition(winSize.width / 2 + 20.f, winSize.height / 2 + 30.f);
    this->m_pLayer->addChild(btn6);
    
    auto btn7 = createBtn("edit_rightBtn_001.png", "1/2", .8f);
    btn7->setPosition(winSize.width / 2 + 60.f, winSize.height / 2 + 30.f);
    this->m_pLayer->addChild(btn7);
}

MoveInfoPopup* MoveInfoPopup::create() {
    auto ret = new MoveInfoPopup;

    if (ret && ret->init(300.f, 220.f, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
