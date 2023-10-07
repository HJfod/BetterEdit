#include "PlaytestHerePopup.hpp"

bool PlaytestHerePopup::init(LevelEditorLayer* lel, StartPosObject* startPos, std::function<void(StartPosKind)> callback) {
    if (!CCNode::init())
        return false;

    m_callback = callback;    
    m_editor = lel;
    m_startPos = startPos;

    this->setZOrder(0xB00B5);

    auto bg = CCScale9Sprite::create("square02_001.png", { 0, 0, 80, 80 });
    bg->setScale(.5f);
    bg->setOpacity(80);
    bg->setContentSize({ 120, 60 });
    this->addChild(bg);

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);

    auto playTestBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
    playTestBtnSpr->setScale(.615f);

    auto playTestBtn = CCMenuItemSpriteExtra::create(
        playTestBtnSpr, this, menu_selector(PlaytestHerePopup::onPlaytest)
    );
    playTestBtn->setPosition(-15.f, .0f);
    menu->addChild(playTestBtn);

    auto playInGameBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    playInGameBtnSpr->setScale(.3f);

    auto playInGameBtn = CCMenuItemSpriteExtra::create(
        playInGameBtnSpr, this, menu_selector(PlaytestHerePopup::onPlayInGame)
    );
    playInGameBtn->setPosition(15.f, .0f);
    menu->addChild(playInGameBtn);

    this->addChild(menu);

    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    menu->registerWithTouchDispatcher();

    s_popup = this;

    return true;
}

void PlaytestHerePopup::onPlaytest(CCObject*) {
    this->select();
    m_callback(m_startPos);
    m_editor->setStartPosObject(m_startPos);
    EditorUI::get()->onPlaytest(EditorUI::get()->m_playtestBtn);
}

void PlaytestHerePopup::onPlayInGame(CCObject*) {
    this->select();
    log::info("startposition: {}, {}", m_startPos->getPositionX(), m_startPos->getPositionY());
    m_editor->m_editorUI->onPause(nullptr);
    EditorPauseLayer::get()->setVisible(false);
    EditorPauseLayer::get()->onSaveAndPlay(nullptr);
}

void PlaytestHerePopup::select() {
    m_callback(m_startPos->getPosition());
}