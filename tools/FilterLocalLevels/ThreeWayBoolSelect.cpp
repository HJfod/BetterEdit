#include "ThreeWayBoolSelect.hpp"

#define BGIFY(name, bg, scale)                                  \
    auto bg_##name = CCSprite::create(bg);                      \
    bg_##name->setScale(scale);                                 \
    bg_##name->addChild(name);                                  \
    bg_##name->setOpacity(100);                                 \
    bg_##name->setColor(cc3x(0x210));                           \
    name->setPosition(bg_##name->getContentSize() / 2);         \

#define SELECT_THING(name, spr, posx, scale, scale2)                \
    auto spr_##name = spr;                                          \
    spr_##name->setOpacity(150);                                    \
    spr_##name->setColor(cc3x(0x9));                                \
    spr_##name->setScale(scale);                                    \
    auto spr_##name##_s = spr;                                      \
    spr_##name##_s->setScale(scale);                                \
    BGIFY(spr_##name, "square02c_001.png", scale2);                 \
    BGIFY(spr_##name##_s, "square02c_001.png", scale2);             \
                                                                    \
    this->m_p##name##Btn = CCMenuItemToggler::create(               \
        bg_spr_##name, bg_spr_##name##_s,                           \
        this,                                                       \
        menu_selector(ThreeWayBoolSelect::onSelect)                 \
    );                                                              \
    this->m_p##name##Btn->setTag(kThreeWayBool##name);              \
    this->m_p##name##Btn->setPosition(posx, -10.f);                 \
    this->addChild(this->m_p##name##Btn);                           \


bool ThreeWayBoolSelect::init(const char* text, CCObject* target, SEL_ThreeWayBool cb) {
    if (!CCMenu::init())
        return false;

    this->m_pTarget = target;
    this->m_callback = cb;
    
    this->m_pLabel = CCLabelBMFont::create(text, "bigFont.fnt");
    this->m_pLabel->limitLabelWidth(50.f, .5f, .1f);
    this->m_pLabel->setPosition(0, 10.f);
    this->addChild(this->m_pLabel);

    // auto spr = CCScale9Sprite::create("square02b_small.png", { 0, 0, 40, 40 });
    // spr->setContentSize({ 120.f, 40.f });
    // spr->setScale(.5f);
    // spr->setZOrder(-1);
    // spr->setOpacity(80);
    // spr->setColor({ 230, 161, 142 });
    // spr->setPositionY(-10.f);
    // this->addChild(spr);

    SELECT_THING(
        False,
        CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
        -20.f, 1.7f, .2f
    );
    SELECT_THING(
        Either,
        CCLabelBMFont::create("-", "bigFont.fnt"),
        0.f, 1.7f, .2f
    );
    SELECT_THING(
        True,
        CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"),
        20.f, 1.7f, .2f
    );

    this->setValue(kThreeWayBoolEither);

    return true;
}

void ThreeWayBoolSelect::onSelect(CCObject* pSender) {
    auto nState = static_cast<ThreeWayBool>(pSender->getTag());

    this->m_pEitherBtn->toggle(false);
    this->m_pFalseBtn->toggle(false);
    this->m_pTrueBtn->toggle(false);

    this->m_eState = nState;

    if (this->m_callback) {
        (this->m_pTarget->*m_callback)(this, nState);
    }
}

void ThreeWayBoolSelect::setValue(ThreeWayBool b) {
    this->m_pEitherBtn->toggle(b == kThreeWayBoolEither);
    this->m_pFalseBtn->toggle(b == kThreeWayBoolFalse);
    this->m_pTrueBtn->toggle(b == kThreeWayBoolTrue);
}

ThreeWayBoolSelect* ThreeWayBoolSelect::create(const char* text, CCObject* target, SEL_ThreeWayBool cb) {
    auto ret = new ThreeWayBoolSelect;

    if (ret && ret->init(text, target, cb)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
