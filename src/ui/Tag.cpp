#include "Tag.hpp"

bool Tag::init(std::string const& text) {
    if (!CCNode::init())
        return false;

    auto bg = CCSprite::createWithSpriteFrameName("border-square.png"_spr);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(.35f);
    bg->addChild(label);

    bg->setContentSize({ label->getScaledContentSize().width + 10.f, 25.f });

    this->addChild(bg);

    return true;
}

Tag* Tag::create(std::string const& text) {
    auto ret = new Tag;
    if (ret && ret->init(text)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
