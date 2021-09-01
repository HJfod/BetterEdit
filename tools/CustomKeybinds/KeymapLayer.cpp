#include "KeymapLayer.hpp"

CCMenuItemSpriteExtra* KeymapLayer::getKeyButton(const char* key, int tag, CCPoint const& pos) {
    auto bspr = ButtonSprite::create(
        key, 24, true, "goldFont.fnt", "GJ_button_05.png", 0x28, .5f
    );
    bspr->setScale(.8f);
    bspr->setTag(tag);
    m_pKeyBtns->addObject(bspr);

    auto btn = CCMenuItemSpriteExtra::create(bspr, this, nullptr);

    btn->setPosition(pos);
    btn->useAnimationType(kMenuAnimationTypeMove);
    btn->setDestination({ 0.0f, 15.0f });

    return btn;
}

CCMenuItemToggler* KeymapLayer::getKeyToggle(const char* key, int width, SEL_MenuHandler cb, CCPoint const& pos) {
    auto bspr = ButtonSprite::create(
        key, width, width, "goldFont.fnt", "GJ_button_05.png", 0x28, .6f
    );
    bspr->setScale(.8f);
    
    auto bspr2 = ButtonSprite::create(
        key, width, width, "goldFont.fnt", "GJ_button_02.png", 0x28, .6f
    );
    bspr2->setScale(.8f);

    auto btn = CCMenuItemToggler::create(bspr, bspr2, this, cb);

    btn->setPosition(pos);

    return btn;
}

void KeymapLayer::setup() {
    this->m_bNoElasticity = true;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pKeyBtns = CCArray::create();
    this->m_pKeyBtns->retain();

    auto map = std::vector<std::string> {
        "QWERTYUIOP",
        "ASDFGHJKL",
        "ZXCVBNM",
    };
    auto ix = 0;
    for (auto const& k : map) {
        auto ypos = 40.0f - ix / static_cast<float>(map.size() - 1) * 80.0f;

        auto i = 0;
        for (auto c : k) {
            auto xpos = i / static_cast<float>(9) * 320.0f - 160.0f + ix * 16.0f;

            this->m_pButtonMenu->addChild(
                this->getKeyButton(
                    std::string(1, c).c_str(), c, { xpos, ypos }
                )
            );

            i++;
        }

        ix++;
    }

    this->m_pButtonMenu->addChild(
        this->getKeyButton("Esc", KEY_Escape, {
            -180.0f, 80.0f
        })
    );

    this->m_pButtonMenu->addChild(this->getKeyToggle("Caps\nLock", 25, nullptr, {
        -180.0f, 0.0f
    }));

    this->m_pButtonMenu->addChild(this->getKeyToggle("Shift", 40, nullptr, {
        -175.0f, -40.0f
    }));

    this->m_pButtonMenu->addChild(this->getKeyToggle("Ctrl", 30, nullptr, {
        -185.0f, -80.0f
    }));
    
    this->m_pButtonMenu->addChild(this->getKeyToggle("Alt", 40, nullptr, {
        -137.5f, -80.0f
    }));
    
    this->m_pButtonMenu->addChild(this->getKeyToggle("Space", 160, nullptr, {
        -40.0f, -80.0f
    }));

    this->m_pButtonMenu->addChild(this->getKeyToggle("PgUp", 25, nullptr, {
        60.0f, -80.0f
    }));
}

KeymapLayer::~KeymapLayer() {
    this->m_pKeyBtns->release();
}

void KeymapLayer::keyDown(enumKeyCodes key) {
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
        btn->updateBGImage(
            key == btn->getTag() ?
                "GJ_button_02.png" : 
                "GJ_button_05.png"
        );
    }
}

void KeymapLayer::keyUp(enumKeyCodes key) {
    CCARRAY_FOREACH_B_TYPE(this->m_pKeyBtns, btn, ButtonSprite) {
        btn->updateBGImage("GJ_button_05.png");
    }
}

KeymapLayer* KeymapLayer::create() {
    auto ret = new KeymapLayer;

    if (ret && ret->init(440.0f, 280.0f, "GJ_square02.png", "Keys")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
