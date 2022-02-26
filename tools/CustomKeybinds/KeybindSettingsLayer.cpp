#include "KeybindSettingsLayer.hpp"

void KeybindSettingsLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;
    
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Double-Click Speed (ms)", "bigFont.fnt")
            .scale(.4f)
            .moveR(winSize, 0.0f, -25.0f)
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(100.0f, "ms"))
            .exec([this](auto i) -> void {
                i->setString(std::to_string(
                    KeybindManager::get()->getDoubleClickInterval()
                ).c_str());
                i->getInputNode()->setTag(2);
                i->getInputNode()->setDelegate(this);
            })
            .moveR(winSize, 0.0f, -50.0f)
            .done()
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

        case 2: {
            if (input->getString() && strlen(input->getString())) {
                try {
                KeybindManager::get()->setDoubleClickInterval(std::stoi(input->getString()));
                } catch (...) {}
            }
        } break;
    }
}

KeybindSettingsLayer* KeybindSettingsLayer::create() {
    auto ret = new KeybindSettingsLayer;

    if (
        ret &&
        ret->init(260.0f, 220.0f, "GJ_square01.png", "Keybind Settings")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
