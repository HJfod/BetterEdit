#include "KeymapLayer.hpp"

KeymapLayer::Layout g_eLayout = KeymapLayer::kLayoutQWERTY;

uint8_t count_ones(uint8_t byte) {
  static const uint8_t NIBBLE_LOOKUP [16] = {
        0, 1, 1, 2, 1, 2, 2, 3, 
        1, 2, 2, 3, 2, 3, 3, 4
  };

  return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

std::string KeymapLayer::layoutToString(Layout l) {
    switch (l) {
        case kLayoutQWERTY: return "QWERTY";
        case kLayoutQWERTZ: return "QWERTZ";
        default: return "Unknown";
    }
}

std::string KeymapLayer::getShortenedKeyName(enumKeyCodes key) {
    switch (key) {
        case KEY_CapsLock:  return "Caps";
        case KEY_Control:   return "Ctrl";
        case KEY_Escape:    return "Esc";
        case KEY_Backspace: return "<-";
        case KEY_Insert:    return "Ins";
        case KEY_Delete:    return "Del";
        case KEY_PageUp:    return "Pg\nUp";
        case KEY_PageDown:  return "Pg\nDown";
        case KEY_Numlock:   return "Num";
        case KEY_ScrollLock:return "Scrl";
        default:            return keyToStringFixed(key);
    }
}

CCMenuItemSpriteExtra* KeymapLayer::getKeyButton(
    const char* key,
    int tag,
    int width,
    CCPoint const& pos,
    bool sprite
) {
    if (!width) width = 12;

    ButtonSprite* bspr;
    if (sprite && strlen(key) > 2) {
        bspr = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(key),
            width, true, .7f, 1,
            "GJ_button_04.png", false, 0x1c
        );
        bspr->m_pBGSprite->setContentSize({
            static_cast<float>(width * 2.33f),
            28.0f
        });
        bspr->m_pSubSprite->setScale(.7f);
    } else {
        bspr = ButtonSprite::create(
            key, width, true, "bigFont.fnt", "GJ_button_04.png", 0x1c, .6f
        );
        bspr->m_pLabel->setColor(m_colText);
    }
    bspr->setScale(.8f);
    bspr->setUserData(as<void*>(tag));
    bspr->setCascadeColorEnabled(true);
    bspr->setCascadeOpacityEnabled(true);
    bspr->m_pBGSprite->setColor(m_colNormal);
    if (bspr->m_pLabel && bspr->m_pLabel->getScale() < .2f)
        bspr->m_pLabel->setScale(.2f);
    m_vKeyBtns.push_back(bspr);

    auto btn = CCMenuItemSpriteExtra::create(
        bspr, this, menu_selector(KeymapLayer::onSelect)
    );

    btn->setPosition(pos);
    btn->m_fScaleMultiplier = .8f;

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
        key, width, width, "bigFont.fnt", "GJ_button_04.png", 0x1c, .8f
    );
    bspr->setCascadeColorEnabled(true);
    bspr->setCascadeOpacityEnabled(true);
    bspr->m_pBGSprite->setColor(m_colToggleable);
    bspr->m_pLabel->setColor(m_colText);
    bspr->setScale(.8f);
    
    auto bspr2 = ButtonSprite::create(
        key, width, width, "bigFont.fnt", "GJ_button_02.png", 0x1c, .8f
    );
    bspr2->setCascadeColorEnabled(true);
    bspr2->setCascadeOpacityEnabled(true);
    bspr2->m_pBGSprite->setColor(m_colToggled);
    bspr2->m_pLabel->setColor(m_colText);
    bspr2->setScale(.8f);

    auto btn = CCMenuItemToggler::create(bspr, bspr2, this, cb);

    btn->setPosition(pos);

    btn->m_pOnButton->m_fScaleMultiplier = .8f;
    btn->m_pOffButton->m_fScaleMultiplier = .8f;

    if (toggle)
        *toggle = btn;

    return btn;
}

void KeymapLayer::loadLayout(Layout l) {
    this->m_eLayout = l;
    g_eLayout = l;
    this->m_pSelectedLayoutLabel->setString(this->layoutToString(l).c_str());
    this->m_pSelectedLayoutLabel->limitLabelWidth(40.f, .4f, .1f);
    this->m_pSelected = nullptr;

    for (auto btn : this->m_vKeyBtns)
        btn->getParent()->removeFromParent();
    this->m_vKeyBtns.clear();

    CATCH_NULL(this->m_pControlToggle)->removeFromParent();
    CATCH_NULL(this->m_pShiftToggle)->removeFromParent();
    CATCH_NULL(this->m_pAltToggle)->removeFromParent();

    switch (l) {
        case kLayoutQWERTY: default:
            this->m_mKeymap = std::vector<std::vector<keybtn>> {
                { { KEY_Escape, 2.5f }, { 12, "F", KEY_F1 }, KEY_Print, KEY_ScrollLock, KEY_Pause, },
                { 1.0f, "1234567890"_s, { KEY_Backspace, 2.5f }, 1.0f, KEY_Insert, KEY_Home, KEY_PageUp,
                    1.0f, KEY_Numlock, KEY_Divide, KEY_Multiply, },
                { { KEY_Tab, 1.25f }, "QWERTYUIOP"_s, { KEY_Enter, 2.5f }, .75f, KEY_Delete, KEY_End, KEY_PageDown,
                    1.0f, KEY_OEMMinus, KEY_OEMPlus, },
                { { KEY_CapsLock, 1.75f }, "ASDFGHJKL"_s, },
                { { KEY_Shift, 2.25f, &m_pShiftToggle }, "ZXCVBNM"_s, KEY_OEMComma, KEY_OEMPeriod, 
                    6.75f, { "edit_upBtn_001.png", KEY_Up }, },
                { { KEY_Control, 2.f, &m_pControlToggle }, { KEY_Alt, 2.f, &m_pAltToggle }, { KEY_Space, 8.f }, 
                    10.0f,
                        { "edit_leftBtn_001.png", KEY_Left },
                        { "edit_downBtn_001.png", KEY_Down },
                        { "edit_rightBtn_001.png", KEY_Right }, 
                    },
            };
            break;

        case kLayoutQWERTZ:
            this->m_mKeymap = std::vector<std::vector<keybtn>> {
                { { KEY_Escape, 2.5f }, { 12, "F", KEY_F1 }, KEY_Print, KEY_ScrollLock, KEY_Pause, },
                { 1.0f, "1234567890"_s, { KEY_Backspace, 2.5f }, 1.0f, KEY_Insert, KEY_Home, KEY_PageUp,
                    1.0f, KEY_Numlock, KEY_Divide, KEY_Multiply, },
                { { KEY_Tab, 1.25f }, "QWERTZUIOP"_s, { KEY_Enter, 2.5f }, .75f, KEY_Delete, KEY_End, KEY_PageDown,
                    1.0f, KEY_OEMMinus, KEY_OEMPlus, },
                { { KEY_CapsLock, 1.75f }, "ASDFGHJKL"_s, },
                { { KEY_Shift, 2.25f, &m_pShiftToggle }, "YXCVBNM"_s, KEY_OEMComma, KEY_OEMPeriod, 
                    6.75f, { "edit_upBtn_001.png", KEY_Up }, },
                { { KEY_Control, 2.f, &m_pControlToggle }, { KEY_Alt, 2.f, &m_pAltToggle }, { KEY_Space, 8.f }, 
                    10.0f,
                        { "edit_leftBtn_001.png", KEY_Left },
                        { "edit_downBtn_001.png", KEY_Down },
                        { "edit_rightBtn_001.png", KEY_Right }, 
                    },
            };
            break;
    }
    
    auto m_ix = 0;
    for (auto const& m : this->m_mKeymap) {
        auto ypos = (s_fItemSeparation + s_fItemWidth) * 4 - m_ix % 6 * (s_fItemSeparation + s_fItemWidth);

        auto xpos = - (s_fItemSeparation + s_fItemWidth) * 10;
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
                } else if (k.key != KEY_None) {
                    this->m_pButtonMenu->addChild(
                        this->getKeyButton(k.text.c_str(), k.key, k.width, {
                            static_cast<float>(xpos + k.width / 2),
                            static_cast<float>(ypos)
                        }, k.sprite)
                    );
                }

                xpos += k.width + s_fItemSeparation;
            }
        }

        m_ix++;
    }

    this->updateKeys();
}

void KeymapLayer::onSwitchLayout(CCObject* pSender) {
    auto add = as<int>(as<CCNode*>(pSender)->getUserData());

    if (!this->m_vLayouts.size()) return;

    auto ix = m_nSelectedLayout + add;

    if (ix < 0) ix = this->m_vLayouts.size() - 1;
    if (ix >= static_cast<int>(this->m_vLayouts.size())) ix = 0;

    this->m_nSelectedLayout = ix;
    this->loadLayout(this->m_vLayouts[this->m_nSelectedLayout]);
}

void KeymapLayer::setup() {
    this->m_bNoElasticity = true;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_vLayouts = { kLayoutQWERTY, kLayoutQWERTZ, };

    this->m_pSelectedLayoutLabel = CCLabelBMFont::create("", "bigFont.fnt");
    this->m_pSelectedLayoutLabel->setPosition({
        winSize.width / 2 + m_pLrSize.width / 2 - 50.0f,
        winSize.height / 2 - m_pLrSize.height / 2 + 30.0f
    });
    this->m_pLayer->addChild(this->m_pSelectedLayoutLabel);
    
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.45f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&KeymapLayer::onSwitchLayout
        ))
        .udata(1)
        .move(m_pLrSize.width / 2 - 20.0f, -m_pLrSize.height / 2 + 30.0f)
        .done()
    );
    
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.45f)
                .done(),
            this,
            (SEL_MenuHandler)&KeymapLayer::onSwitchLayout
        ))
        .udata(-1)
        .move(m_pLrSize.width / 2 - 80.0f, -m_pLrSize.height / 2 + 30.0f)
        .done()
    );

    auto bg = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bg->setPosition({ winSize.width / 2, winSize.height / 2 - 100.0f });
    bg->setContentSize({ 560.0f, 60.0f });
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

    this->m_pSelectedKeybindLabel = BGLabel::create("", "goldFont.fnt");
    this->m_pSelectedKeybindLabel->setScale(.65f);
    this->m_pSelectedKeybindLabel->setBGOpacity(50);
    this->m_pSelectedKeybindLabel->setPosition(
        winSize.width / 2, winSize.height / 2 - 47.5f
    );
    this->m_pSelectedKeybindLabel->setVisible(false);
    this->m_pLayer->addChild(this->m_pSelectedKeybindLabel);

    auto rebindBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor<ButtonSprite*>()
            .fromButtonSprite("Set Bind", "GJ_button_01.png", "goldFont.fnt")
            .scale(.6f)
            .done(),
        this,
        menu_selector(KeymapLayer::onSetBind)
    );
    rebindBtn->setPosition(35.0f, -70.0f);
    rebindBtn->setVisible(false);
    this->m_pButtonMenu->addChild(rebindBtn);
    this->m_pBindBtns.push_back(rebindBtn);

    auto unbindBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor<ButtonSprite*>()
            .fromButtonSprite("Unbind", "GJ_button_06.png", "goldFont.fnt")
            .scale(.6f)
            .done(),
        this,
        menu_selector(KeymapLayer::onUnbind)
    );
    unbindBtn->setPosition(-35.0f, -70.0f);
    unbindBtn->setVisible(false);
    this->m_pButtonMenu->addChild(unbindBtn);
    this->m_pBindBtns.push_back(unbindBtn);
    
    this->m_pSelected = nullptr;

    this->loadLayout(g_eLayout);

    MAKE_INFOBUTTON(
        "Keymap",
        
        "Use the <cp>keymap</c> to quickly see which keys are already assigned "
        "and assign new keybinds\n\n "

        "Some keys are <cr>missing</c>, because they are <cy>not supported</c> "
        "by <cb>Cocos2d</c> :(",
        1.0f,
        this->m_pLrSize.width / 2 - 30.0f,
        this->m_pLrSize.height / 2 - 30.0f,
        this->m_pButtonMenu
    );
}

void KeymapLayer::onToggle(CCObject*) {
    this->m_bUpdateOnNextVisit = true;
}

void KeymapLayer::onSetBind(CCObject*) {
    if (this->m_pKeybindingsLayer && this->m_pSelected) {
        this->m_pKeybindingsLayer->setSelectMode(
            true,
            this->getKeybind()
        );
        this->onClose(nullptr);
    }
}

void KeymapLayer::onUnbind(CCObject*) {
    FLAlertLayer::create(
        this,
        "Confirm Unbind",
        "Cancel", "Unbind",
        "Delete <cr>all</c> binds associated with this "
        "<cl>keybind</c>?"
    )->show();
}

void KeymapLayer::FLAlert_Clicked(FLAlertLayer*, bool btn2) {
    if (btn2 && this->m_pSelected) {
        KeybindManager::get()->clearCallbacks(this->getKeybind());

        this->onSelect(this->m_pSelected->getParent());
        this->loadKeybind(KEY_None);
        this->updateKeys();

        if (this->m_pKeybindingsLayer)
            this->m_pKeybindingsLayer->reloadList();
    }
}

void KeymapLayer::onSelect(CCObject* pSender) {
    auto b = as<ButtonSprite*>(
        as<CCMenuItemSpriteExtra*>(pSender)->getNormalImage()
    );

    if (!this->m_pKeybindingsLayer) return;

    if (this->m_pSelected && this->m_pSelected->m_pBGSprite) {
        auto csize = this->m_pSelected->m_pBGSprite->getContentSize();
        this->m_pSelected->updateBGImage("GJ_button_04.png");
        this->m_pSelected->m_pBGSprite->setContentSize(csize);
        this->updateKey(this->m_pSelected);
    }

    if (b == this->m_pSelected) {
        this->m_pSelected = nullptr;
    } else {
        this->m_pSelected = b;
        auto csize = this->m_pSelected->m_pBGSprite->getContentSize();
        b->updateBGImage("GJ_button_02.png");
        this->m_pSelected->m_pBGSprite->setContentSize(csize);
    }
    
    for (auto btn : this->m_pBindBtns)
        btn->setVisible(this->m_pSelected);
    
    this->m_pSelectedKeybindLabel->setString(this->getKeybind().toString().c_str());
    this->m_pSelectedKeybindLabel->setVisible(this->m_pSelected);
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
    if (key == KEY_None && this->m_pSelected)
        key = enum_cast<enumKeyCodes>(this->m_pSelected->getUserData());

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
    for (auto btn : m_vKeyBtns) {
        if (key == as<int>(btn->getUserData())) {
            btn->stopAllActions();
            btn->runAction(
                CCEaseBackOut::create(CCScaleTo::create(.3f, .4f))
            );
        }
    }

    this->m_ePressed.insert(key);
}

void KeymapLayer::keyUp(enumKeyCodes key) {
    for (auto btn : m_vKeyBtns) {
        if (key == as<int>(btn->getUserData())) {
            btn->stopAllActions();
            btn->runAction(
                CCEaseBackOut::create(CCScaleTo::create(.3f, .8f))
            );
        }
    }

    this->m_ePressed.erase(key);
}

void KeymapLayer::updateKey(ButtonSprite* btn) {
    auto k = this->getKeybind(enum_cast<enumKeyCodes>(btn->getUserData()));

    auto cb = KeybindManager::get()->getAllCallbacksForKeybind(k);

    btn->m_pBGSprite->setColor(m_colNormal);

    if (btn->m_pBGSprite && cb.size()) {
        enum { eui = 1, pl = 2, gb = 4, };
        int target = 0;
        bool mul = false;
        for (auto const& t : cb)
            switch (t.type) {
                case kKBEditor:
                    if (target & eui) mul = true;
                    target |= eui;
                    break;
                case kKBPlayLayer:
                    if (target & pl) mul = true;
                    target |= pl;
                    break;
                case kKBGlobal:
                    if (target & gb) mul = true;
                    target |= gb;
                    break;
            }

        ccColor3B col = m_colNormal;
        if (mul) {
            col = m_colConflict;
        } else {
            if (target & eui) col = m_colInUseEditor;
            if (target & pl)  col = m_colInUsePlay;
            if (target & gb)  col = m_colInUseGlobal;
            if (count_ones(target) >= 2) col = m_colInUse;
        }

        btn->m_pBGSprite->setColor(col);
    }
}

void KeymapLayer::updateKeys() {
    for (auto btn : m_vKeyBtns)
        if (btn != this->m_pSelected)
            this->updateKey(btn);

    this->m_pSelectedKeybindLabel->setString(this->getKeybind().toString().c_str());
    this->m_pSelectedKeybindLabel->setVisible(this->m_pSelected);
}

void KeymapLayer::showHoveredKey() {
    auto mpos = getMousePos();

    this->m_pCallbackLabel->setString("");

    bool isHovering = false;
    for (auto btn : m_vKeyBtns) {

        auto pos = btn->getParent()->convertToWorldSpace(btn->getPosition());
        auto size = btn->getScaledContentSize();

        auto rect = CCRect {
            pos.x - size.width / 2,
            pos.y - size.height / 2,
            size.width,
            size.height
        };

        btn->setOpacity(100);
        if (
            rect.containsPoint(mpos) ||
            this->m_ePressed.find(enum_cast<enumKeyCodes>(btn->getUserData())) !=
                this->m_ePressed.end()
        ) {
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

        for (auto btn : m_vKeyBtns) {
            btn->setOpacity(255);
        }
    }
}

void KeymapLayer::loadKeybind(Keybind const& kb) {
    for (auto spr : m_vKeyBtns) {
        if (enum_cast<enumKeyCodes>(spr->getUserData()) == kb.key)
            this->onSelect(spr->getParent());
    }

    this->m_pControlToggle->toggleWithCallback(kb.modifiers & kb.kmControl);
    this->m_pShiftToggle->toggleWithCallback(kb.modifiers & kb.kmShift);
    this->m_pAltToggle->toggleWithCallback(kb.modifiers & kb.kmAlt);
}

KeymapLayer* KeymapLayer::create(KeybindingsLayer_CB* layer) {
    auto ret = new KeymapLayer;

    if (
        ret &&
        ((ret->m_pKeybindingsLayer = layer) || true) &&
        ret->init(480.0f, 280.0f, "GJ_square02.png", "Keys")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
