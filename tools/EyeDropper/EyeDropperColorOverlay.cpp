#include "EyeDropperColorOverlay.hpp"

bool EyeDropperColorOverlay::init(ColorSelectPopup* popup) {
    if (!CCNode::init())
        return false;

    this->m_pTargetPopup = popup;
    this->m_pTargetPopup->retain();
    this->m_pTargetAction = popup->colorAction;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pSprite = ColorChannelSprite::create();
    this->m_pSprite->setPosition({ 40.0f, winSize.height - 40.0f });

    this->addChild(this->m_pSprite);


    this->m_pLabelBG = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    this->m_pLabelBG->setOpacity(80);
    this->m_pLabelBG->setContentSize({ 0, 0 });
    this->m_pLabelBG->setPosition(winSize.width / 2, winSize.height - 40.0f);
    this->m_pLabelBG->setColor({ 0, 0, 0 });
    this->m_pLabelBG->setScale(.25f);

    this->addChild(this->m_pLabelBG);


    this->m_pLabel = CCLabelBMFont::create("Right-Click to Pick", "bigFont.fnt");

    this->m_pLabel->setScale(.4f);
    this->m_pLabel->setPosition(winSize.width / 2, winSize.height - 40.0f);
    this->m_pLabel->setOpacity(180);

    this->addChild(this->m_pLabel);


    auto menu = CCMenu::create();

    auto closeBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_closeBtn_001.png")
            .scale(.6f)
            .done(),
        this,
        (SEL_MenuHandler)&EyeDropperColorOverlay::onCancel
    );

    menu->addChild(closeBtn);

    menu->setPosition(80.0f, winSize.height - 40.0f);
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(4);
    menu->registerWithTouchDispatcher();

    this->addChild(menu);

    return true;
}

void EyeDropperColorOverlay::onCancel(CCObject*) {
    this->finish(nullptr);
}

void EyeDropperColorOverlay::finish(ccColor3B* color) {
    if (m_pStatusBool)
        *m_pStatusBool = false;

    if (color) {
        if (m_pTargetPopup)
            m_pTargetPopup->setPickerColor(*color);

        else if (m_pTargetAction)
            m_pTargetAction->m_color = *color;
    }

    if (m_pTargetPopup)
        m_pTargetPopup->release();
    this->removeFromParentAndCleanup(true);
}

void EyeDropperColorOverlay::setStatusBool(bool* statusBool) {
    this->m_pStatusBool = statusBool;
}

void EyeDropperColorOverlay::setShowColor(ccColor3B const& color) {
    this->m_pSprite->setColor(color);
}

void EyeDropperColorOverlay::setLabelText(bool inWindow) {
    this->m_pLabel->setString(
        inWindow ? "Right-Click to Pick" :
        "Click Any Button to Pick Screen Color"
    );
    this->m_pLabelBG->setContentSize(
        this->m_pLabel->getScaledContentSize() * 4 + CCSize { 50.0f, 50.0f }
    );
}

EyeDropperColorOverlay* EyeDropperColorOverlay::addToCurrentScene(ColorSelectPopup* popup) {
    auto overlay = create(popup);
    auto scene = CCDirector::sharedDirector()->getRunningScene();

    overlay->setZOrder(scene->getHighestChildZ() + 106);

    scene->addChild(overlay);

    return overlay;
}

EyeDropperColorOverlay* EyeDropperColorOverlay::create(ColorSelectPopup* popup) {
    auto pRet = new EyeDropperColorOverlay;

    if (pRet && pRet->init(popup)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
