#include "StartPosButtonBar.hpp"

bool StartPosButtonBar::init(LevelEditorLayer* lel) {
    if (!CCMenu::init())
        return false;
    
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
        m_obContentSize.height / 2 + 20.f
    );
    this->addChild(goToStartBtn);

    auto counterLabel = CCLabelBMFont::create("", "bigFont.fnt");
    counterLabel->setScale(0.6f);
    counterLabel->setPosition({ m_obContentSize.width / 2, m_obContentSize.height / 2 - 25.f });
    counterLabel->limitLabelWidth(60.0f, 0.6f, 0.1f);
    counterLabel->setString("0 / 0");
    m_counterLabel = counterLabel;
    this->addChild(counterLabel);
    this->setStartPosCounters();

    auto goToLastSpr = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("last-start-pos.png"_spr),
        0x32, true, 0x32, "GJ_button_01.png", .75f
    );
    auto goToLastBtn = CCMenuItemSpriteExtra::create(
        goToLastSpr, this, menu_selector(StartPosButtonBar::onGoToLast)
    );
    goToLastBtn->setPosition(
        m_obContentSize.width / 2 + 25.f,
        m_obContentSize.height / 2 + 20.f
    );
    this->addChild(goToLastBtn);

    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(0.8f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(StartPosButtonBar::onInfo)
    );
    infoBtn->setPosition(m_obContentSize / 2 + ccp(65.f, 20.f));
    this->addChild(infoBtn);

    auto arrowLeft = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    arrowLeft->setScale(0.7f);
    auto leftbutton = CCMenuItemSpriteExtra::create(
        arrowLeft, this, menu_selector(StartPosButtonBar::onPrevious)
    );
    leftbutton->setPosition(m_obContentSize / 2 + ccp(-45.0f, -25.0f));
    this->addChild(leftbutton);

    auto arrowRight = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    arrowRight->setScale(0.7f);
    arrowRight->setFlipX(true);
    auto rightButton = CCMenuItemSpriteExtra::create(
        arrowRight, this, menu_selector(StartPosButtonBar::onNext)
    );
    rightButton->setPosition(m_obContentSize / 2 + ccp(45.0f, -25.0f));
    this->addChild(rightButton);

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
    StartPosManager::get()->setFirst();
    this->setStartPosCounters();
}

void StartPosButtonBar::onGoToLast(CCObject*) {
    StartPosManager::get()->setDefault();
    this->setStartPosCounters();
}

void StartPosButtonBar::setStartPosCounters() {
    m_totalStartPositions = 0;
    m_activeIndex = 0;
    if (!m_editor || !m_editor->m_objects) {
        return;
    }

    m_totalStartPositions = StartPosManager::get()->getPositions().size() - 1;
    auto active = StartPosManager::get()->getActive();

    bool found = false;

    if (StartPosManager::get()->isDefault()) {
        m_activeIndex = m_totalStartPositions;
        found = true;
    }

    if (StartPosManager::get()->isLevelStart() && !found) {
        m_activeIndex = 0;
        found = true;
    }

    if (!found) {
        int i = 0;
        for (auto pos : StartPosManager::get()->getPositions()) {
            if (pos == active) {
                m_activeIndex = i;
            }
            i++;
        }
    }

    std::string str = std::to_string(m_activeIndex) + " / " + std::to_string(m_totalStartPositions);

    m_counterLabel->setString(str.c_str());
    m_counterLabel->limitLabelWidth(60.0f, 0.6f, 0.1f);
}

void StartPosButtonBar::onNext(CCObject*) {
    if (m_activeIndex == m_totalStartPositions) {
        return;
    }

    StartPosManager::get()->next();
    this->setStartPosCounters();
}

void StartPosButtonBar::onPrevious(CCObject*) {
    if (m_activeIndex == 0) {
        return;
    }

    StartPosManager::get()->previous();
    this->setStartPosCounters();
}