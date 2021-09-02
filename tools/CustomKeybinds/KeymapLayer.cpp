#include "KeymapLayer.hpp"

CCMenuItemSpriteExtra* KeymapLayer::getKeyButton(
    const char* key,
    int tag,
    int width,
    CCPoint const& pos
) {
    if (!width) width = 12;

    auto bspr = ButtonSprite::create(
        key, width, true, "goldFont.fnt", "GJ_button_05.png", 0x1c, .6f
    );
    bspr->setScale(.8f);
    bspr->setUserData(as<void*>(tag));
    bspr->setCascadeColorEnabled(true);
    bspr->setCascadeOpacityEnabled(true);
    m_pKeyBtns->addObject(bspr);

    auto btn = CCMenuItemSpriteExtra::create(bspr, this, nullptr);

    btn->setPosition(pos);

    btn->m_bColorEnabled = true;
    btn->m_bAnimationEnabled = false;

    return btn;
}

CCMenuItemToggler* KeymapLayer::getKeyToggle(
    const char* key,
    int width,
    CCMenuItemToggler** toggle,
    SEL_MenuHandler cb,
    CCPoint const& pos
) {
    auto bspr = ButtonSprite::create(
        key, width, width, "goldFont.fnt", "GJ_button_05.png", 0x1c, .6f
    );
    bspr->setCascadeColorEnabled(true);
    bspr->setCascadeOpacityEnabled(true);
    bspr->setScale(.8f);
    
    auto bspr2 = ButtonSprite::create(
        key, width, width, "goldFont.fnt", "GJ_button_02.png", 0x1c, .6f
    );
    bspr2->setCascadeColorEnabled(true);
    bspr2->setCascadeOpacityEnabled(true);
    bspr2->setScale(.8f);

    auto btn = CCMenuItemToggler::create(bspr, bspr2, this, cb);

    btn->setPosition(pos);

    btn->m_pOnButton->m_bColorEnabled = true;
    btn->m_pOnButton->m_bAnimationEnabled = false;

    btn->m_pOffButton->m_bColorEnabled = true;
    btn->m_pOffButton->m_bAnimationEnabled = false;

    if (toggle)
        *toggle = btn;

    return btn;
}

void KeymapLayer::setup() {
    this->m_bNoElasticity = true;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pKeyBtns = CCArray::create();
    this->m_pKeyBtns->retain();

    auto map = std::vector<std::string> {
        "1234567890",
        "QWERTYUIOP",
        "ASDFGHJKL",
        "ZXCVBNM",
    };
    auto ix = 0;
    for (auto const& k : map) {
        auto ypos = m_fItemSeparation * 2 - ix / static_cast<float>(map.size() - 1) *
            (m_fItemSeparation * map.size());

        auto i = 0;
        for (auto c : k) {
            auto xpos = i / static_cast<float>(9) *
                m_fItemSeparation * 11 - m_fItemSeparation * 6 +
                ix * (m_fItemSeparation / 2.f);

            this->m_pButtonMenu->addChild(
                this->getKeyButton(
                    std::string(1, c).c_str(), c, 0, { xpos, ypos }
                )
            );

            i++;
        }

        ix++;
    }

    this->m_pButtonMenu->addChild(
        this->getKeyButton("Esc", KEY_Escape, 0, {
            - m_fItemSeparation * 7.25f,
            m_fItemSeparation * 3.5f
        })
    );

    for (auto i = 0; i < 12; i++) {
        this->m_pButtonMenu->addChild(
            this->getKeyButton(("F"_s + std::to_string(i + 1)).c_str(),
                static_cast<enumKeyCodes>(KEY_F1 + i), 0,
                {
                    i / static_cast<float>(10) * m_fItemSeparation * 12 -
                        m_fItemSeparation * 6,
                    m_fItemSeparation * 3.5f
                }
            )
        );
    }

    this->m_pButtonMenu->addChild(
        this->getKeyButton("Tab", KEY_Tab, 24, {
            -m_fItemSeparation * 7.f, m_fItemSeparation * .65f
        })
    );

    this->m_pButtonMenu->addChild(
        this->getKeyButton("Caps\nLock", KEY_CapsLock, 35, {
            -m_fItemSeparation * 6.75f, -m_fItemSeparation * .65f
        })
    );

    this->m_pButtonMenu->addChild(this->getKeyToggle("Shift", 45, &this->m_pShiftToggle,
        menu_selector(KeymapLayer::onToggle), {
            -m_fItemSeparation * 7 + 7.5f, -m_fItemSeparation * 2.0f
        }
    ));

    this->m_pButtonMenu->addChild(this->getKeyToggle("Ctrl", 30, &this->m_pControlToggle,
        menu_selector(KeymapLayer::onToggle), {
            -m_fItemSeparation * 7, -m_fItemSeparation * 3.5f
        }
    ));
    
    this->m_pButtonMenu->addChild(this->getKeyToggle("Alt", 40, &this->m_pAltToggle,
        menu_selector(KeymapLayer::onToggle), {
            -m_fItemSeparation * 4.75f, -m_fItemSeparation * 3.5f
        }
    ));
    
    this->m_pButtonMenu->addChild(
        this->getKeyButton("Space", KEY_Space, static_cast<int>(m_fItemSeparation * 5), {
            -m_fItemSeparation * 1, -m_fItemSeparation * 3.5f
        })
    );

    this->m_pButtonMenu->addChild(
        this->getKeyButton("PgUp", KEY_PageUp, 25, {
            m_fItemSeparation * 2.5f, -m_fItemSeparation * 3.5f
        })
    );

    this->m_pButtonMenu->addChild(
        this->getKeyButton("PgDown", KEY_PageDown, 25, {
            m_fItemSeparation * 4.5f, -m_fItemSeparation * 3.5f
        })
    );

    this->m_pButtonMenu->addChild(
        this->getKeyButton("<", KEY_Left, 0, {
            m_fItemSeparation * 6.f, -m_fItemSeparation * 3.5f
        })
    );

    this->m_pButtonMenu->addChild(
        this->getKeyButton(">", KEY_Right, 0, {
            m_fItemSeparation * 8.f, -m_fItemSeparation * 3.5f
        })
    );

    auto btnDown = 
        this->getKeyButton("<", KEY_Down, 0, {
            m_fItemSeparation * 7.f, -m_fItemSeparation * 3.5f
        });
    as<ButtonSprite*>(btnDown->getNormalImage())->m_pLabel->setRotation(270);
    this->m_pButtonMenu->addChild(btnDown);

    auto btnUp = 
        this->getKeyButton(">", KEY_Up, 0, {
            m_fItemSeparation * 7.f, -m_fItemSeparation * 2.5f
        });
    as<ButtonSprite*>(btnUp->getNormalImage())->m_pLabel->setRotation(270);
    this->m_pButtonMenu->addChild(btnUp);

    auto bg = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bg->setPosition({ winSize.width / 2, winSize.height / 2 - 100.0f });
    bg->setContentSize({ 600.0f, 60.0f });
    bg->setScale(.5f);
    bg->setOpacity(75);
    bg->setColor({ 0, 0, 0 });
    this->m_pLayer->addChild(bg);

    this->m_pCallbackLabel = TextArea::create(
        "bigFont.fnt", false, "", .4f, 300.0f, 30.0f, { .9f, 0.0f }
    );
    this->m_pCallbackLabel->setPosition({ winSize.width / 2, winSize.height / 2 - 100.0f });
    this->m_pCallbackLabel->setZOrder(4);
    this->m_pLayer->addChild(this->m_pCallbackLabel);

    this->updateKeys();
}

KeymapLayer::~KeymapLayer() {
    this->m_pKeyBtns->release();
}

void KeymapLayer::onToggle(CCObject*) {
    this->updateKeys();
}

void KeymapLayer::visit() {
    this->BrownAlertDelegate::visit();

    auto kb = CCDirector::sharedDirector()->getKeyboardDispatcher();

    if (this->m_bControlDown != kb->getControlKeyPressed()) {
        this->m_bControlDown = kb->getControlKeyPressed();
        this->m_pControlToggle->toggle(this->m_bControlDown);
        this->updateKeys();
    }

    if (this->m_bShiftDown != kb->getShiftKeyPressed()) {
        this->m_bShiftDown = kb->getShiftKeyPressed();
        this->m_pShiftToggle->toggle(this->m_bShiftDown);
        this->updateKeys();
    }

    if (this->m_bAltDown != kb->getAltKeyPressed()) {
        this->m_bAltDown = kb->getAltKeyPressed();
        this->m_pAltToggle->toggle(this->m_bAltDown);
        this->updateKeys();
    }
    
    this->showHoveredKey();
}

void KeymapLayer::keyDown(enumKeyCodes key) {
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
        if (key == as<int>(btn->getUserData()))
            btn->updateBGImage("GJ_button_02.png");
    }
}

void KeymapLayer::keyUp(enumKeyCodes key) {
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
        if (key == as<int>(btn->getUserData())) {
            if (this->m_obHovered.key == key)
                btn->updateBGImage("GJ_button_01.png");
            else
                btn->updateBGImage("GJ_button_05.png");
        }
    }
}

void KeymapLayer::updateKeys() {
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
        btn->updateBGImage("GJ_button_05.png");
    }

    for (auto const& [bind, cb] : KeybindManager::get()->getAllKeybinds()) {
        CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
            auto k = Keybind(enum_cast<enumKeyCodes>(btn->getUserData()));

            if (k == bind) {
                btn->updateBGImage("GJ_button_01.png");
            }
        }
    }
}

void KeymapLayer::showHoveredKey() {
    auto mpos = getMousePos();

    this->m_pCallbackLabel->setString("");

    bool isHovering = false;
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {

        auto pos = btn->getParent()->convertToWorldSpace(btn->getPosition());
        auto size = btn->getScaledContentSize();

        auto rect = CCRect {
            pos.x - size.width / 2,
            pos.y - size.height / 2,
            size.width,
            size.height
        };

        btn->setOpacity(100);
        if (rect.containsPoint(mpos)) {
            std::string str = "";

            auto k = Keybind(enum_cast<enumKeyCodes>(btn->getUserData()));

            isHovering = true;

            if (m_obHovered == k) {
                btn->setOpacity(255);
                return this->m_pCallbackLabel->setString(m_sLabelString.c_str());
            }

            for (auto const& bind : KeybindManager::get()
                ->getCallbacksForKeybind(kKBPlayLayer, k)
            )
                str += "<cy>" + bind->name + "</c>, ";

            for (auto const& bind : KeybindManager::get()
                ->getCallbacksForKeybind(kKBEditor, k)
            )
                str += "<cg>" + bind->name + "</c>, ";
                
            for (auto const& bind : KeybindManager::get()
                ->getCallbacksForKeybind(kKBGlobal, k)
            )
                str += "<cp>" + bind->name + "</c>, ";

            str = str.substr(0, str.size() - 2);

            m_sLabelString = str;

            btn->setOpacity(255);
            this->m_pCallbackLabel->setString(str);

            m_obHovered = k;
        }
    }

    if (!isHovering)
        this->m_sLabelString = "";

    if (!m_sLabelString.size()) {
        this->m_obHovered.key = KEY_None;

        CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
            btn->setOpacity(255);
        }
    }
}

KeymapLayer* KeymapLayer::create() {
    auto ret = new KeymapLayer;

    if (ret && ret->init(460.0f, 280.0f, "GJ_square02.png", "Keys")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
