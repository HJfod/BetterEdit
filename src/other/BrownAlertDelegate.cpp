#include "BrownAlertDelegate.hpp"

bool BrownAlertDelegate::init(float _w, float _h, const char* _spr, const char* _title) {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    this->m_pLrSize = cocos2d::CCSize { _w, _h };

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    this->m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(this->m_mainLayer);

    auto bg = cocos2d::extension::CCScale9Sprite::create(_spr, { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(this->m_pLrSize);
    bg->setPosition(winSize.width / 2, winSize.height / 2);
    this->m_mainLayer->addChild(bg);
    this->m_pBGSprite = bg;

    this->m_buttonMenu = cocos2d::CCMenu::create();
    this->m_mainLayer->addChild(this->m_buttonMenu);

    if (sizeof _title > 0) {
        auto title = cocos2d::CCLabelBMFont::create(_title, "goldFont.fnt");

        title->limitLabelWidth(this->m_pLrSize.width * 4, .75f, .2f);
        title->setPosition(
            winSize.width / 2,
            winSize.height / 2 + this->m_pLrSize.height / 2 - 25
        );

        this->m_mainLayer->addChild(title);
    }

    setup();

    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();
    
    auto closeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(1.0f);

    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeSpr,
        this,
        (cocos2d::SEL_MenuHandler)&BrownAlertDelegate::onClose
    );
    closeBtn->setUserData(reinterpret_cast<void*>(this));

    this->m_buttonMenu->addChild(closeBtn);

    closeBtn->setPosition( - _w / 2, _h / 2 );

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}

void BrownAlertDelegate::keyDown(cocos2d::enumKeyCodes key) {
    if (key == cocos2d::enumKeyCodes::KEY_Escape)
        return onClose(nullptr);
    if (key == cocos2d::enumKeyCodes::KEY_Space)
        return;
    
    return FLAlertLayer::keyDown(key);
}

void BrownAlertDelegate::onClose(cocos2d::CCObject* pSender) {
    this->setKeyboardEnabled(false);
    this->removeFromParentAndCleanup(true);
};