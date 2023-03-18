#include "Tag.hpp"

bool Tag::init(std::string const& text, std::optional<std::string> const& icon) {
    if (!CCNode::init())
        return false;

    auto bg = CCScale9Sprite::createWithSpriteFrameName("border-square.png"_spr);
    this->addChild(bg);

    float width = 5.f;
    float height = 18.f;

    if (icon) {
        auto spr = CCSprite::createWithSpriteFrameName(icon.value().c_str());
        spr->setScale(.45f);
        spr->setPosition({ width + spr->getScaledContentSize().width / 2, height / 2 });
        this->addChild(spr);
        width += spr->getScaledContentSize().width;
        width += 5.f;
    }

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(.45f);
    label->setPosition({ width, height / 2 });
    label->setAnchorPoint({ .0f, .5f });
    width += label->getScaledContentSize().width;
    this->addChild(label);

    width += 5.f;

    this->setContentSize({ width, height });

    bg->setScale(.5f);
    bg->setContentSize(m_obContentSize / bg->getScale());
    bg->setPosition(m_obContentSize / 2);

    return true;
}

Tag* Tag::create(std::string const& text, std::optional<std::string> const& icon) {
    auto ret = new Tag;
    if (ret && ret->init(text, icon)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
