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
        m_obContentSize.height / 2
    );
    this->addChild(goToStartBtn);

    auto goToLastSpr = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("last-start-pos.png"_spr),
        0x32, true, 0x32, "GJ_button_01.png", .75f
    );
    auto goToLastBtn = CCMenuItemSpriteExtra::create(
        goToLastSpr, this, menu_selector(StartPosButtonBar::onGoToLast)
    );
    goToLastBtn->setPosition(
        m_obContentSize.width / 2 + 25.f,
        m_obContentSize.height / 2
    );
    this->addChild(goToLastBtn);

    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(StartPosButtonBar::onInfo)
    );
    infoBtn->setPosition(m_obContentSize / 2 + ccp(60.f, 0.f));
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
    if (m_callback) {
        m_callback(FromLevelStart());
    }
}

void StartPosButtonBar::onGoToLast(CCObject*) {
    StartPosObject* last = nullptr;
    for(auto* obj : cocos::CCArrayExt<StartPosObject>(m_editor->m_objects)) {
        if (obj->m_objectID == 31) {
            if (!last || last->getPositionX() < obj->getPositionX()) {
                last = obj;
            }
        }
    }
    if (last) {
        log::info("calling last");
        m_callback(last);
    } else {
        log::info("default");
        m_callback(DefaultBehaviour());
    }
}