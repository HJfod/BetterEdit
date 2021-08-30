#include "KeybindSettingsLayer.hpp"

void KeybindSettingsLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;

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
                i->setString(
                    BetterEdit::getKeybindRepeatIntervalAsString().c_str()
                );
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
                i->setString(
                    BetterEdit::getKeybindRepeatStartAsString().c_str()
                );
                i->getInputNode()->setTag(1);
                i->getInputNode()->setDelegate(this);
            })
            .moveR(winSize, 0.0f, -25.0f)
            .done()
    );

    GameToolbox::createToggleButton(
        menu_selector(KeybindSettingsLayer::onEnable),
        BetterEdit::getKeybindRepeatEnabled(),
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
                (SEL_MenuHandler)&KeybindSettingsLayer::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void KeybindSettingsLayer::onClose(CCObject* pSender) {
    KeybindManager::get()->resetUnmodifiedRepeatTimes();

    BrownAlertDelegate::onClose(pSender);
}

void KeybindSettingsLayer::onEnable(CCObject* pSender) {
    BetterEdit::setKeybindRepeatEnabled(
        !as<CCMenuItemToggler*>(pSender)->isToggled()
    );
}

void KeybindSettingsLayer::textChanged(CCTextInputNode* input) {
    switch (input->getTag()) {
        case 0: {
            if (input->getString() && strlen(input->getString())) {
                try {
                BetterEdit::setKeybindRepeatIntervalFromString(input->getString());
                } catch (...) {}
            }
        } break;

        case 1: {
            if (input->getString() && strlen(input->getString())) {
                try {
                BetterEdit::setKeybindRepeatStartFromString(input->getString());
                } catch (...) {}
            }
        } break;
    }
}

KeybindSettingsLayer* KeybindSettingsLayer::create() {
    auto ret = new KeybindSettingsLayer;

    if (
        ret &&
        ret->init(220.0f, 240.0f, "GJ_square01.png", "Keybind Settings")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
