#include "MoveInfoPopup.hpp"

CCMenuItemSpriteExtra* createBtn(
    CCLayer* target,
    const char* spr,
    const char* sizeTxt,
    float scale,
    CCPoint const& position,
    const char* text
) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

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
    btn->setPosition(winSize / 2 + position);

    target->addChild(btn);

    auto label = CCLabelBMFont::create(text, "bigFont.fnt", 70.f, kCCTextAlignmentCenter);
    label->setScale(.25f);
    label->setColor({ 50, 200, 255 });
    label->setPosition(winSize / 2 + position - CCPoint { .0f, 24.0f });
    target->addChild(label);

    return btn;
}

void MoveInfoPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createBtn(
        this->m_pLayer, "edit_rightBtn_001.png", nullptr, 1.f,
        { -90.f, 45.f }, "1/15 block\n2 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn2_001.png", nullptr, 1.f,
        { - 30.f, 45.f }, "1 block\n30 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn3_001.png", nullptr, 1.f,
        { + 30.f, 45.f }, "5 blocks\n120 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn_001.png", nullptr, .8f,
        { + 90.f, 45.f }, "1/60 block\n0.5 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn2_001.png", "1/2", 1.f,
        { - 90.f, - 35.f }, "1/2 block\n15 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn2_001.png", "1/4", 1.f,
        { - 30.f, - 35.f }, "1/4 block\n7.5 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn2_001.png", "1/8", 1.f,
        { + 30.f, - 35.f }, "1/8 block\n3.75 units"
    );
    createBtn(
        this->m_pLayer, "edit_rightBtn_001.png", "1/2", .8f,
        { + 90.f, - 35.f }, "1/120 block\n0.25 units"
    );
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
