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

    struct keybtn {
        enumKeyCodes key;
        std::string text = "";
        CCMenuItemToggler** target = nullptr;
        int width;
        std::vector<keybtn> sub;

        keybtn(enumKeyCodes k) {
            key = k;
            width = s_fItemWidth;
            text = keyToStringFixed(k);
        }

        keybtn(enumKeyCodes k, CCMenuItemToggler** b) {
            key = k;
            width = s_fItemWidth;
            text = keyToStringFixed(k);
            target = b;
        }

        keybtn(enumKeyCodes k, std::string const& t) {
            key = k;
            text = t;
            width = s_fItemWidth;
        }

        keybtn(enumKeyCodes k, float w) {
            key = k;
            width = static_cast<int>(s_fItemWidth * w);
            text = keyToStringFixed(k);
        }

        keybtn(std::string const& keys) {
            key = KEY_None;

            for (auto const& key : keys)
                sub.push_back({ static_cast<enumKeyCodes>(key) });
        }

        keybtn(int keys, std::string const& prefix, enumKeyCodes start) {
            key = KEY_None;

            for (auto ix = 0; ix < keys; ix++)
                sub.push_back({
                    static_cast<enumKeyCodes>(start + ix),
                    prefix + std::to_string(ix + 1)
                });
        }
    };

    auto map = std::initializer_list<std::initializer_list<keybtn>> {
        { KEY_Escape, { 12, "F", KEY_F1 }, KEY_Print, KEY_ScrollLock, KEY_Pause, },
        { "1234567890"_s, { KEY_Backspace, 1.25f }, KEY_Insert, KEY_Home, KEY_PageUp,
            KEY_Numlock, KEY_Divide, KEY_Multiply, KEY_OEMMinus, },
        { KEY_Tab, "QWERTYUIOP"_s, KEY_Enter, KEY_Delete, KEY_End, KEY_PageDown,
            KEY_NumPad7, KEY_NumPad8, KEY_NumPad9, KEY_OEMPlus, },
        { KEY_CapsLock, "ASDFGHJKL"_s,
            KEY_NumPad5, KEY_NumPad6, KEY_NumPad7, },
        { { KEY_Shift, &m_pShiftToggle }, "ZXCVBNM"_s, KEY_OEMComma, KEY_OEMPeriod, 
            KEY_NumPad1, KEY_NumPad2, KEY_NumPad3, },
        { { KEY_Control, &m_pControlToggle }, { KEY_Alt, &m_pAltToggle }, { KEY_Space, 2.5f }, 
            KEY_Left, KEY_Up, KEY_Right, KEY_Down, KEY_NumPad0, },
    };

    auto m_ix = 0;
    for (auto const& m : map) {
        auto ypos = (s_fItemSeparation + s_fItemWidth) * 4 - m_ix % 6 * (s_fItemSeparation + s_fItemWidth);

        auto xpos = - (s_fItemSeparation + s_fItemWidth) * 9;
        for (auto const& k : m) {
            if (k.sub.size()) {
                for (auto const& s : k.sub) {
                    this->m_pButtonMenu->addChild(
                        this->getKeyButton(s.text.c_str(), s.key, s.width, {
                            static_cast<float>(xpos + s.width / 2),
                            static_cast<float>(ypos)
                        })
                    );

                    xpos += s.width + s_fItemSeparation;
                }
            } else {
                if (k.target) {
                    this->m_pButtonMenu->addChild(
                        this->getKeyToggle(k.text.c_str(), k.width, k.target,
                            menu_selector(KeymapLayer::onToggle), {
                            static_cast<float>(xpos + k.width / 2),
                            static_cast<float>(ypos)
                        })
                    );
                } else {
                    this->m_pButtonMenu->addChild(
                        this->getKeyButton(k.text.c_str(), k.key, k.width, {
                            static_cast<float>(xpos + k.width / 2),
                            static_cast<float>(ypos)
                        })
                    );
                }

                xpos += k.width + s_fItemSeparation;
            }
        }

        m_ix++;
    }

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
    this->m_bUpdateOnNextVisit = true;
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
    
    if (this->m_bUpdateOnNextVisit) {
        this->updateKeys();
        this->m_bUpdateOnNextVisit = false;
    }

    this->showHoveredKey();
}

Keybind KeymapLayer::getKeybind(enumKeyCodes key) {
    auto k = Keybind(key);
    k.modifiers = 0;

    if (this->m_pControlToggle->isToggled())
        k.modifiers |= k.kmControl;
    if (this->m_pShiftToggle->isToggled())
        k.modifiers |= k.kmShift;
    if (this->m_pAltToggle->isToggled())
        k.modifiers |= k.kmAlt;
    
    return k;
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
            auto k = this->getKeybind(enum_cast<enumKeyCodes>(btn->getUserData()));

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

            auto k = this->getKeybind(enum_cast<enumKeyCodes>(btn->getUserData()));

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
