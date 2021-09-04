#include "KeybindRepeatPopup.hpp"

void KeybindRepeatPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;

    auto subTitle = CCLabelBMFont::create(m_pCell->m_pBind->name.c_str(), "bigFont.fnt");
    subTitle->setColor({ 50, 155, 255 });
    subTitle->setPosition(
        winSize.width / 2,
        winSize.height / 2 + this->m_pLrSize.height / 2 - 45.0f
    );
    subTitle->setScale(.5f);
    this->m_pLayer->addChild(subTitle);

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Interval (ms)", "bigFont.fnt")
            .scale(.4f)
            .moveR(winSize, 0.0f, 55.0f)
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(100.0f, "ms"))
            .exec([this](auto i) -> void {
                i->setString(BetterEdit::formatToString(
                    this->m_pCell->m_pBind->repeatInterval
                ).c_str());
                i->getInputNode()->setTag(0);
                i->getInputNode()->setDelegate(this);
            })
            .moveR(winSize, 0.0f, 30.0f)
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Start (ms)", "bigFont.fnt")
            .scale(.4f)
            .moveR(winSize, 0.0f, 0.0f)
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(100.0f, "ms"))
            .exec([this](auto i) -> void {
                i->setString(BetterEdit::formatToString(
                    this->m_pCell->m_pBind->repeatStart
                ).c_str());
                i->getInputNode()->setTag(1);
                i->getInputNode()->setDelegate(this);
            })
            .moveR(winSize, 0.0f, -25.0f)
            .done()
    );

    GameToolbox::createToggleButton(
        menu_selector(KeybindRepeatPopup::onEnable),
        this->m_pCell->m_pBind->repeat,
        this->m_pButtonMenu, this,
        this->m_pLayer, .7f, .4f,
        120.0f, "", false, 0,
        nullptr, "Enable Repeating",
        { winSize.width / 2 - 65.0f, winSize.height / 2 - 60.0f },
        { 5.0f, .0f }
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "OK", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&KeybindRepeatPopup::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );

    MAKE_INFOBUTTON(
        "Keybind Repeating",
        
        "<cp>Repeating</c> means that when you hold down a shortcut, "
        "it will start repeating after a certain amount of time. "
        "For example, you can hold to move an object continuously instead "
        "of spamming the shortcut to do it.\n\n "
        "<cg>Interval:</c> How fast the shortcut should be repeated\n "
        "<cy>Start:</c> How long should the shortcut be held down until "
        "repeating starts\n ",
        .65f,
        this->m_pLrSize.width / 2 - 25.0f,
        - this->m_pLrSize.height / 2 + 25.0f,
        this->m_pButtonMenu
    );
}

void KeybindRepeatPopup::onEnable(CCObject* pSender) {
    this->m_pCell->m_pBind->repeat =
        !as<CCMenuItemToggler*>(pSender)->isToggled();

    this->m_pCell->m_pBind->repeatChanged = true;
    this->m_pCell->updateMenu();
}

void KeybindRepeatPopup::textChanged(CCTextInputNode* input) {
    switch (input->getTag()) {
        case 0: {
            if (input->getString() && strlen(input->getString())) {
                try {
                this->m_pCell->m_pBind->repeatInterval =
                    std::stoi(input->getString());
                } catch (...) {}
            }
        } break;

        case 1: {
            if (input->getString() && strlen(input->getString())) {
                try {
                this->m_pCell->m_pBind->repeatStart =
                    std::stoi(input->getString());
                } catch (...) {}
            }
        } break;
    }

    this->m_pCell->m_pBind->repeatChanged = true;
    this->m_pCell->updateMenu();
}

KeybindRepeatPopup* KeybindRepeatPopup::create(KeybindCell* cell) {
    auto ret = new KeybindRepeatPopup;

    if (
        ret &&
        ((ret->m_pCell = cell) || true) &&
        ret->init(220.0f, 240.0f, "GJ_square01.png", "Repeat Keybind")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
