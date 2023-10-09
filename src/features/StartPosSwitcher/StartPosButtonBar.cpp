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
        m_obContentSize.height / 2 + 20.f
    );
    this->addChild(goToStartBtn);

    auto counterLabel = CCLabelBMFont::create("", "bigFont.fnt");
    counterLabel->setScale(0.6f);
    counterLabel->setPosition({ m_obContentSize.width / 2, m_obContentSize.height / 2 - 25.f });
    counterLabel->limitLabelWidth(80.0f, 0.6f, 0.4f);
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
    m_callback(FromLevelStart());
    this->setStartPosCounters(FromLevelStart());
}

void StartPosButtonBar::onGoToLast(CCObject*) {
    m_callback(DefaultBehaviour());
    this->setStartPosCounters(DefaultBehaviour());
}

void StartPosButtonBar::setStartPosCounters(StartPosKind startPos) {
    m_totalStartPositions = 0;
    m_activeIndex = 0;
    if (!m_editor || !m_editor->m_objects) {
        return;
    }

    CCArrayExt<StartPosObject*> startPositions;

    for (auto object : CCArrayExt<GameObject*>(m_editor->m_objects)) {
        if (object->m_objectID != 31) {
            continue;
        }

        startPositions.push_back(static_cast<StartPosObject*>(object));
        m_totalStartPositions++;
    }

    if (std::holds_alternative<FromPoint>(startPos) && m_activeIndex == 0) {
        auto pos = std::get<FromPoint>(startPos);
        std::sort(startPositions.begin(), startPositions.end(), [](StartPosObject* x, StartPosObject* y){
            return x->getPositionX() < y->getPositionX();
        });
        int i = 0;
        for (auto object : startPositions) {
            i++;
            if (object->getPosition() == pos || object->getPosition() == m_editor->m_editorUI->getGridSnappedPos(pos)) {
                if (object->getPosition() == pos) {
                    m_activeIndex = i;
                    m_active = startPos;
                } else if (!m_editor->m_editorInitialising && object->getPosition() == m_editor->m_editorUI->getGridSnappedPos(pos)) {
                    m_activeIndex = i;
                    m_active = startPos;
                }
            }
        }
    }

    if (std::holds_alternative<DefaultBehaviour>(startPos)) {
        m_activeIndex = m_totalStartPositions;
        m_active = startPos;
    }

    if (std::holds_alternative<FromLevelStart>(startPos)) {
        m_activeIndex = 0;
        m_active = startPos;
    }

    std::string str = std::to_string(m_activeIndex) + " / " + std::to_string(m_totalStartPositions);

    m_counterLabel->setString(str.c_str());
}

void StartPosButtonBar::onNext(CCObject*) {
    if (m_activeIndex == m_totalStartPositions) {
        return;
    }
    if (std::holds_alternative<DefaultBehaviour>(m_active)) {
        return;
    }

    log::info("active: {}", m_activeIndex);
    auto found = this->search(m_active, FindHigher);
    m_callback(found);
    this->setStartPosCounters(found);
}

void StartPosButtonBar::onPrevious(CCObject*) {
    if (m_activeIndex == 0) {
        return;
    }
    if (std::holds_alternative<FromLevelStart>(m_active)) {
        return;
    }
    if (m_activeIndex == 1) {
        m_active = FromLevelStart();
        m_callback(m_active);
        this->setStartPosCounters(m_active);
    }

    auto found = this->search(m_active, FindSmaller);
    m_callback(found);
    this->setStartPosCounters(found);
}

StartPosKind StartPosButtonBar::search(StartPosKind current, StartPositionSearchType searchType) {
    std::vector<CCPoint> positions;

    for (auto object : CCArrayExt<GameObject*>(m_editor->m_objects)) {
        if (object->m_objectID == 31) {
            switch (searchType) {
                case StartPositionSearchType::FindHigher: {
                    if (std::holds_alternative<DefaultBehaviour>(current)) {
                        return current;
                    }
                    if (std::holds_alternative<FromLevelStart>(current)) {
                        positions.push_back(object->getPosition());
                        continue;
                    }
                    if (object->getPositionX() > std::get<FromPoint>(current).x) {
                        positions.push_back(object->getPosition());
                    }
                } break;
                case StartPositionSearchType::FindSmaller: {
                    if (std::holds_alternative<FromLevelStart>(current)) {
                        return current;
                    }
                    if (std::holds_alternative<DefaultBehaviour>(current)) {
                        positions.push_back(object->getPosition());
                        continue;
                    }
                    if (object->getPositionX() < std::get<FromPoint>(current).x) {
                        positions.push_back(object->getPosition());
                    }
                } break;
            }
        }
    }

    for (auto pos : positions) {
        log::info("position: {}, {}", pos.x, pos.y);
    }

    if (searchType == StartPositionSearchType::FindSmaller) {
        positions.push_back(CCPointZero);
    }

    if (positions.size() == 0) {
        return current;
    }

    switch (searchType) {
        case StartPositionSearchType::FindHigher: {
            std::sort(positions.begin(), positions.end(), [](CCPoint first, CCPoint second) {
                return first.x < second.x;
            });

            return positions.at(0);
        } break;
        case StartPositionSearchType::FindSmaller: {
            std::sort(positions.begin(), positions.end(), [](CCPoint first, CCPoint second) {
                return first.x > second.x;
            });

            if (std::holds_alternative<DefaultBehaviour>(current) && positions.size() > 1) {
                return positions.at(1);
            }

            if (positions.at(0) == CCPointZero) {
                return FromLevelStart();
            }
            return positions.at(0);
        } break;
    }
    return current;
}