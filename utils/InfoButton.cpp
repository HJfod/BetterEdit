#include "InfoButton.hpp"

bool InfoButton::init(
    std::string const& title,
    std::string const& desc,
    float scale
) {
    auto spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    spr->setScale(scale);

    if (!CCMenuItemSprite::initWithNormalSprite(spr, nullptr, nullptr, this, nullptr))
        return false;
    if (!CCMenuItemSpriteExtra::init(spr))
        return false;
    
    this->m_sTitle = title;
    this->m_sDescription = desc;

    return true;
}

void InfoButton::activate() {
    FLAlertLayer::create(
        nullptr,
        m_sTitle.c_str(),
        "OK", nullptr,
        400.0f,
        m_sDescription
    )->show();
}

InfoButton* InfoButton::create(
    std::string const& title,
    std::string const& desc,
    float scale
) {
    auto ret = new InfoButton;

    if (ret && ret->init(title, desc, scale)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
