#include "StartPosButtonBar.hpp"

bool StartPosButtonBar::init(LevelEditorLayer* lel, std::function<void(StartPosKind)> changeCallback) {
    if (!CCMenu::init())
        return false;
    
    m_callback = changeCallback;
    m_editor = lel;

    auto winSize = CCDirector::get()->getWinSize();

    this->ignoreAnchorPointForPosition(false);
    this->setPosition(winSize.width / 2, 45.f);
    this->setContentSize({ 200.f, 60.f });
    
    auto goToStartSpr = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("start-from-start.png"_spr),
        0x32, true, 0x32, "GJ_button_01.png", .7f
    );
    auto goToStartBtn = CCMenuItemSpriteExtra::create(
        goToStartSpr, this, menu_selector(StartPosButtonBar::onGoToStart)
    );
    goToStartBtn->setPosition(
        m_obContentSize.width / 2 - 25.f,
        m_obContentSize.height / 2 + 10.f
    );
    this->addChild(goToStartBtn);

    auto counterLabel = CCLabelBMFont::create("", "bigFont.fnt");
    counterLabel->setScale(0.6f);
    counterLabel->setPosition({ m_obContentSize.width / 2, m_obContentSize.height / 2 - 25.f });
    m_counterLabel = counterLabel;
    this->addChild(counterLabel);
    this->setStartPosCounters(DefaultBehaviour());

    auto goToLastSpr = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("last-start-pos.png"_spr),
        0x32, true, 0x32, "GJ_button_01.png", .75f
    );
    auto goToLastBtn = CCMenuItemSpriteExtra::create(
        goToLastSpr, this, menu_selector(StartPosButtonBar::onGoToLast)
    );
    goToLastBtn->setPosition(
        m_obContentSize.width / 2 + 25.f,
        m_obContentSize.height / 2 + 10.f
    );
    this->addChild(goToLastBtn);

    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(0.7f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(StartPosButtonBar::onInfo)
    );
    infoBtn->setPosition(m_obContentSize / 2 + ccp(70.f, 10.f));
    this->addChild(infoBtn);

    return true;
}

void StartPosButtonBar::onInfo(CCObject*) {
    FLAlertLayer::create(
        "Start Pos Info",
        "<cp>From Level Start</c>: Start playing from the beginning of the "
        "level, ignoring all start positions\n"
        "<cb>From Last Start Pos</c>: Start playing from the last start "
        "position in the level",
        "OK"
    )->show();
}

void StartPosButtonBar::onGoToStart(CCObject*) {
    m_callback(FromLevelStart());
    this->setStartPosCounters(FromLevelStart());
}

void StartPosButtonBar::onGoToLast(CCObject*) {
    m_callback(DefaultBehaviour());
    this->setStartPosCounters(DefaultBehaviour());
}

void StartPosButtonBar::setStartPosCounters(StartPosKind startPos) {
    int totalStartPos = 0;
    int activeIndex = 0;
    if (!m_editor || !m_editor->m_objects) {
        return;
    }

    for (auto object : CCArrayExt<GameObject*>(m_editor->m_objects)) {
        if (object->m_objectID != 31) {
            continue;
        }

        totalStartPos++;
        if (std::holds_alternative<FromPoint>(startPos)) {
            auto pos = std::get<FromPoint>(startPos);
            if (object->getPosition() == pos) {
                activeIndex = totalStartPos;
            }
        }
    }

    if (std::holds_alternative<DefaultBehaviour>(startPos)) {
        activeIndex = totalStartPos;
    }

    if (std::holds_alternative<FromLevelStart>(startPos)) {
        activeIndex = 0;
    }

    std::string str = std::to_string(activeIndex) + " / " + std::to_string(totalStartPos);

    m_counterLabel->setString(str.c_str());
}