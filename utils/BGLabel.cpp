#include "BGLabel.hpp"

bool BGLabel::init(const char* text, const char* font) {
    if (!this->CCNode::init())
        return false;
    
    this->m_pBGSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    this->m_pBGSprite->setScale(.5f);
    this->m_pBGSprite->setColor({ 0, 0, 0 });
    this->m_pBGSprite->setOpacity(175);
    this->addChild(this->m_pBGSprite);

    this->m_pLabel = CCLabelBMFont::create(text, font);
    this->m_pLabel->setScale(.6f);
    this->addChild(this->m_pLabel);

    this->updateSize();

    return true;
}

void BGLabel::setString(const char* text) {
    this->m_pLabel->setString(text);
    this->updateSize();
}

void BGLabel::updateSize() {
    this->m_pBGSprite->setContentSize({
        max(strlen(this->m_pLabel->getString()) * 20.f + 20.0f, 80.0f),
        60.f
    });
}

void BGLabel::setBGOpacity(GLubyte alpha) {
    this->m_pBGSprite->setOpacity(alpha);
}

BGLabel* BGLabel::create(const char* text, const char* font) {
    auto pRet = new BGLabel;

    if (pRet && pRet->init(text, font)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
