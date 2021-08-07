#include "EyeDropperColorOverlay.hpp"

bool EyeDropperColorOverlay::init() {
    if (!CCNode::init())
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pSprite = ColorChannelSprite::create();
    this->m_pSprite->setPosition({ 40.0f, winSize.height - 40.0f });

    this->addChild(this->m_pSprite);

    auto label = CCLabelBMFont::create("Right-Click to Pick", "bigFont.fnt");

    label->setScale(.4f);
    label->setPosition(winSize.width / 2, winSize.height - 40.0f);
    label->setOpacity(120);

    this->addChild(label);

    return true;
}

void EyeDropperColorOverlay::setShowColor(ccColor3B const& color) {
    this->m_pSprite->setColor(color);
}

EyeDropperColorOverlay* EyeDropperColorOverlay::addToCurrentScene() {
    auto overlay = create();
    auto scene = CCDirector::sharedDirector()->getRunningScene();

    overlay->setZOrder(scene->getHighestChildZ() + 106);

    scene->addChild(overlay);

    return overlay;
}

EyeDropperColorOverlay* EyeDropperColorOverlay::create() {
    auto pRet = new EyeDropperColorOverlay;

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
