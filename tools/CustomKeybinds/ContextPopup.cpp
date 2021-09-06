#include "ContextPopup.hpp"

bool ContextPopup::init(
    CCPoint const& pos,
    CCSize const& size,
    ContextPopupDirection dir
) {
    if (!CCLayer::init())
        return false;
    
    this->m_obSize = size;
    this->m_eDirection = dir;

    this->m_pBG = CCScale9Sprite::create(
        "GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    this->m_pBG->setContentSize(this->m_obSize);
    this->m_pBG->setPosition(0, 0);
    this->addChild(this->m_pBG);

    auto thumb = CCSprite::create("sliderthumb.png");
    switch (dir) {
        case kContextPopupDirectionDown:
            this->setPosition(
                this->m_obSize.width / 2,
                this->m_obSize.height
            );
    }
    thumb->setPosition();

    this->setup();

    return true;
}
