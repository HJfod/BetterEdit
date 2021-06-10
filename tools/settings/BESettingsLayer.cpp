#include "BESettingsLayer.hpp"

using namespace gdmake;
using namespace gdmake::extra;
using namespace gd;
using namespace cocos2d;

std::string ccToStr(const char* str) {
    return str;
}

void BESettingsLayer::setup() {
    // this->m_pPrevPageBtn = CCMenuItemSpriteExtra::create(
    //     CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
    //     this,
    //     nullptr
    // );
    // this->m_pPrevPageBtn->setPosition(- this->m_pLrSize.width / 2 - 64.0f, 0.0f);
    // this->m_pButtonMenu->addChild(this->m_pPrevPageBtn, 150);

    // this->m_pNextPageBtn = CCMenuItemSpriteExtra::create(
    //     CCSpriteConstructor::fromFrameName("GJ_arrow_01_001.png")->flipX(),
    //     this,
    //     nullptr
    // );
    // this->m_pNextPageBtn->setPosition(this->m_pLrSize.width / 2 + 64.0f, 0.0f);
    // this->m_pButtonMenu->addChild(this->m_pNextPageBtn, 150);

    this->addToggle("hey", nullptr, "desc");
    this->addToggle("another one", nullptr, "desc2");
    this->addToggle("haha this is awesome", nullptr, "desc3");
    this->addToggle("coool", nullptr, "desc4");

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto scaleSnapInput = InputNode::create(50.0f, "4");
    auto scaleSnapLabel = CCLabelBMFont::create("Scale Snap:", "goldFont.fnt");

    scaleSnapInput->setPosition(winSize.width / 2 + 60.0f, winSize.height / 2 - this->m_pLrSize.height / 2 + 34.0f);
    scaleSnapLabel->setPosition(winSize.width / 2 - 30.0f, winSize.height / 2 - this->m_pLrSize.height / 2 + 34.0f);
    scaleSnapLabel->setScale(.7f);

    this->m_pLayer->addChild(scaleSnapInput);
    this->m_pLayer->addChild(scaleSnapLabel);

    CCARRAY_FOREACH_B_TYPE(BetterEdit::sharedState()->getSettingsDict()->allKeys(), key, cocos2d::CCString)
        std::cout << key->getCString() << "\n";

    std::cout << BetterEdit::sharedState()->getKeyInt("scale-snap") << "\n";
    scaleSnapInput->setString(std::to_string(BetterEdit::sharedState()->getKeyInt("scale-snap")).c_str());
}

void BESettingsLayer::addToggle(const char* text, const char* desc, const char* key) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(
        this,
        (SEL_MenuHandler)&BESettingsLayer::onToggle,
        .75f
    );

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->limitLabelWidth(this->m_pLrSize.width / 2 - 60.0f, .6f, .2f);

    auto x = this->m_nToggleCount % 2 ? 0.0f : - this->m_pLrSize.width / 2 + 24.0f;
    auto y = this->m_pLrSize.height / 2 - 64.0f - (this->m_nToggleCount / 2) * 35.0f;

    toggle->setPosition(x, y);
    label->setPosition(x + 16.0f + label->getScaledContentSize().width / 2, y);

    if (key != nullptr) {
        toggle->toggle(BetterEdit::sharedState()->getKeyInt(key));
        toggle->setUserObject(CCString::create(key));
    }

    this->m_pButtonMenu->addChild(toggle);
    this->m_pButtonMenu->addChild(label);

    this->m_nToggleCount++;
}

void BESettingsLayer::onToggle(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);

    if (toggle && toggle->getUserObject())
        BetterEdit::sharedState()->setKeyInt(
            as<CCString*>(toggle->getUserObject())->getCString(),
            !toggle->isToggled()
        );
}

BESettingsLayer* BESettingsLayer::create() {
    auto ret = new BESettingsLayer();

    if (ret && ret->init(340.0f, 240.0f, "GJ_square01.png", "BetterEdit Settings")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
