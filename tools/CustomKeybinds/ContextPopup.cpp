#include "ContextPopup.hpp"

constexpr const float g_fPadding = 20.0f;
constexpr const float g_fDuration = .1f;

void ContextPopup::show() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    scene->addChild(this, scene->getChildrenCount() * 200 + 400);

    this->m_pBG->runAction(CCEaseSineOut::create(
        CCFadeTo::create(g_fDuration, this->m_pBG->getOpacity())
    ));
    this->m_pBG->setOpacity(0);
    this->m_pThumb->runAction(CCEaseSineOut::create(
        CCFadeTo::create(g_fDuration, this->m_pThumb->getOpacity())
    ));
    this->m_pThumb->setOpacity(0);

    this->runAction(CCSequence::create(
        CCSpawn::create(
            CCEaseSineOut::create(
                CCMoveTo::create(g_fDuration, this->m_obDestination)
            ),
            nullptr
        ),
        CCCallFunc::create(
            this,
            callfunc_selector(ContextPopup::onAnimationComplete)
        ),
        nullptr
    ));
}

void ContextPopup::hide() {
    if (!m_bAnimationComplete) return;

    this->m_pBG->runAction(CCEaseSineIn::create(
        CCFadeTo::create(g_fDuration, 0)
    ));
    this->m_pThumb->runAction(CCEaseSineIn::create(
        CCFadeTo::create(g_fDuration, 0)
    ));

    this->runAction(CCSequence::create(
        CCSpawn::create(
            CCEaseSineIn::create(
                CCMoveTo::create(g_fDuration, this->m_obOrigin)
            ),
            nullptr
        ),
        CCCallFunc::create(
            this,
            callfunc_selector(ContextPopup::removeFromParent)
        ),
        nullptr
    ));
}

void ContextPopup::onAnimationComplete() {
    this->m_bAnimationComplete = true;
}

void ContextPopup::mouseLeaveSuper(CCPoint const&) {
    // this->hide();
}

void ContextPopup::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->hide();
}

void ContextPopup::mouseScrollOutsideSuper(float, float) {
    this->hide();
}

bool ContextPopup::mouseScrollSuper(float, float) {
    return true;
}

void ContextPopup::keyDownSuper(enumKeyCodes) {
    this->hide();
}

bool ContextPopup::init(
    CCPoint const& pos,
    CCSize const& size,
    ContextPopupDirection dir,
    ContextPopupType type
) {
    if (!CCNodeRGBA::init())
        return false;
    
    this->m_eType = type;
    this->m_obSize = size;
    this->m_eDirection = dir;
    this->m_obOrigin = pos;

    this->m_bAnimationComplete = false;

    this->setContentSize(size);

    switch (type) {
        case kContextPopupTypeBrown:
            this->m_pBG = CCScale9Sprite::create(
                "GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f }
            );
            m_pThumb = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
            break;
        
        default:
            this->m_pBG = CCScale9Sprite::create(
                "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
            );
            m_pThumb = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    }
    this->m_pBG->setContentSize(this->m_obSize * 2);
    this->m_pBG->setPosition(0.f, 0.f);
    this->m_pBG->setScale(.5f);
    this->addChild(this->m_pBG);

    if (type == kContextPopupTypeBlack) {
        this->m_pBG->setOpacity(160);
        this->m_pBG->setColor(cc3x(0));
        this->m_pThumb->setOpacity(160);
        this->m_pThumb->setColor(cc3x(0));
    }

    m_pThumb->setScale(.65f);
    m_pThumb->setZOrder(1);
    auto tsize = m_pThumb->getScaledContentSize();
    switch (dir) {
        case kContextPopupDirectionDown:
            this->m_obDestination = CCPoint {
                pos.x,
                pos.y + g_fPadding + this->m_obSize.height / 2
            };
            m_pThumb->setPosition({
                0.0f,
                -m_obSize.height / 2 - tsize.height / 2
            });
            m_pThumb->setRotation(0);
            break;
        case kContextPopupDirectionUp:
            this->m_obDestination = CCPoint {
                pos.x,
                pos.y - g_fPadding - this->m_obSize.height / 2
            };
            m_pThumb->setPosition({
                0.0f,
                m_obSize.height / 2 + tsize.height / 2
            });
            m_pThumb->setRotation(180);
            break;
        case kContextPopupDirectionLeft:
            this->m_obDestination = CCPoint {
                pos.x + g_fPadding + this->m_obSize.width / 2,
                pos.y
            };
            m_pThumb->setPosition({
                -m_obSize.width / 2 - tsize.width / 2,
                0.0f 
            });
            m_pThumb->setRotation(270);
            break;
        case kContextPopupDirectionRight:
            this->m_obDestination = CCPoint {
                pos.x - g_fPadding - this->m_obSize.width / 2,
                pos.y
            };
            m_pThumb->setPosition({
                m_obSize.width / 2 + tsize.width / 2,
                0.0f
            });
            m_pThumb->setRotation(90);
            break;
    }
    this->addChild(m_pThumb);

    this->setPosition(pos);

    this->setup();

    return true;
}
