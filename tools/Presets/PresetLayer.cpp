#include "PresetLayer.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void PresetLayer::setup() {
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "Create Empty", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
            ),
            this,
            (SEL_MenuHandler)&PresetLayer::onCreate
        ))
        .move(0.0f, this->m_pLrSize.height / 2 - 30.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "With Alpha Trigger", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
            ),
            this,
            (SEL_MenuHandler)&PresetLayer::onCreate
        ))
        .uobj(CCString::create("H4sIAAAAAAAACqWQ2w3DMAhFFyISF4zjKF-ZoQPcAbJCh69t-tmoqfrDMa8j5PPhTcCiNMKCTosgkLBEFgsXsBKqypUgYoRGZSOe4FSo3VPgf8X2UTFmcuGWxDj2r0XaNeNfvmjiUqO_XFMvNHIecNGBSNREkR7zvWbljTbw8G1mNmMKZuMoM2YXmoDoDulXrGKy2CYuC5qKRc-9CiR6W-a49zP2F0g-5bc8AgAA"))
        .move(0.0f, 0.0f)
        .done()
    );
}

void PresetLayer::onCreate(CCObject* pSender) {
    auto lvlStr = as<CCString*>(as<CCNode*>(pSender)->getUserObject());

    auto level = GameLevelManager::createNewLevel();
    if (lvlStr)
        level->setLevelData(lvlStr->getCString());

    EditLevelLayer::scene(level);
}

PresetLayer* PresetLayer::create() {
    auto pRet = new PresetLayer();

    if (pRet && pRet->init(200.0f, 200.0f)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
