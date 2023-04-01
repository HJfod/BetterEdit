#include "InfoPopup.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/TextArea.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/MDTextArea.hpp>
#include "Label.hpp"

bool InfoPopup::setup(std::string const& title) {
    this->setTitle(title);

    m_content = CCNode::create();
    m_content->setContentSize({ m_size.width - 40.f, m_size.height - 70.f });
    m_content->setPosition(m_obContentSize / 2);
    m_content->setAnchorPoint({ .5f, .5f });
    m_content->setLayout(
        RowLayout::create()
            ->setGrowCrossAxis(true)
            ->setGap(0.f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisLineAlignment(AxisAlignment::End)
    );
    m_mainLayer->addChild(m_content);

    auto okSpr = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png");
    okSpr->setScale(.75f);
    auto okBtn = CCMenuItemSpriteExtra::create(
        okSpr, this, menu_selector(InfoPopup::onClose)
    );
    okBtn->setPosition(0.f, -m_size.height / 2 + 25.f);
    m_buttonMenu->addChild(okBtn);

    return true;
}

void InfoPopup::setDirection(Axis axis) {
    static_cast<AxisLayout*>(m_content->getLayout())
        ->setAxis(axis)
        ->setAxisReverse(true)
        ->setCrossAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setCrossAxisLineAlignment(AxisAlignment::Start);
}

void InfoPopup::addIconText(const char* icon, std::string const& text) {
    auto node = CCNode::create();
    node->setContentSize({ (m_size.width - 40.f) / 2, 32.f });
    auto size = node->getContentSize();

    auto spr = CCSprite::createWithSpriteFrameName(icon);
    spr->setPosition({ size.height / 2, size.height / 2 });
    node->addChild(spr);
    limitNodeSize(spr, { 20.f, 20.f }, 1.f, .1f);

    auto label = Label::create(text, "bigFont.fnt", { size.width - 40.f, size.height });
    label->setMaxScale(.5f);
    label->setAlignment(AxisAlignment::Start);
    label->setPosition({ 32.f, size.height / 2 });
    label->setAnchorPoint({ .0f, .5f });
    node->addChild(label);

    m_content->addChild(node);
    m_content->updateLayout();

    m_last = node;
}

void InfoPopup::addBreak() {
    if (m_last) {
        m_last->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setBreakLine(true)
        );
    }
}

InfoPopup* InfoPopup::create(std::string const& title) {
    auto ret = new InfoPopup;
    if (ret && ret->init(330.f, 250.f, title, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

