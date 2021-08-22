#include "BackupViewLayer.hpp"
#include "LevelBackupManager.hpp"
#include "BackupListView.hpp"
#include "BackupScheduleLayer.hpp"
#include "../../../utils/InfoButton.hpp"

bool BackupViewLayer::init(GJGameLevel* level) {
    if (!CCLayer::init())
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto sRight = CCDirector::sharedDirector()->getScreenRight();
    
    this->m_pLevel = level;

	auto bg = CCSprite::create("GJ_gradientBG.png");
	auto bgSize = bg->getTextureRect().size;

	bg->setAnchorPoint({ 0.0f, 0.0f });
	bg->setScaleX((winSize.width + 10.0f) / bgSize.width);
	bg->setScaleY((winSize.height + 10.0f) / bgSize.height);
	bg->setPosition({ -5.0f, -5.0f });
	bg->setColor({ 0, 102, 255 });

	this->addChild(bg);

    auto bottomLeft = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
	auto cornerSize = bottomLeft->getTextureRect().size;
	bottomLeft->setPosition({ cornerSize.width / 2, cornerSize.height / 2 });
	auto topRight = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
	topRight->setFlipX(true);
	topRight->setFlipY(true);
	topRight->setPosition({ winSize.width - cornerSize.width / 2, winSize.height - cornerSize.height / 2 });
	this->addChild(bottomLeft);
	this->addChild(topRight);

    this->m_pButtonMenu = CCMenu::create();
    this->addChild(this->m_pButtonMenu);

    auto backBtn = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(BackupViewLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	this->m_pButtonMenu->addChild(backBtn);

    auto addBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
        this,
        menu_selector(BackupViewLayer::onNew)
    );
    addBtn->setPosition(winSize.width / 2 - 40.0f, - winSize.height / 2 + 40.0f);
    this->m_pButtonMenu->addChild(addBtn);

    auto timeBtn = CCMenuItemSpriteExtra::create(
        createBESprite("BE_timeBtn_001.png"),
        this,
        menu_selector(BackupViewLayer::onSchedule)
    );
    timeBtn->setPosition(winSize.width / 2 - 40.0f, - winSize.height / 2 + 95.0f);
    this->m_pButtonMenu->addChild(timeBtn);

    m_sTitle = "Backups for " + this->m_pLevel->levelName;

    m_pListLabel = CCLabelBMFont::create("No backups found!", "bigFont.fnt");

    m_pListLabel->setPosition(winSize / 2);
    m_pListLabel->setScale(.6f);
    m_pListLabel->setVisible(false);
    m_pListLabel->setZOrder(1001);

    this->m_pButtonMenu->addChild(m_pListLabel);

    MAKE_INFOBUTTON(
        "Level Backups",
        
        "<cy>Backups enable you to save a copy of your level, "
        "and load it at any time later on.</c>\n "
        "<cb>Backups are saved in a different location from normal "
        "levels, so even if you lose your normal GD data, they should "
        "be recoverable.</c>\n "
        "<co>To create a backup, press the + button in the corner.</c>",
        1.0f,
        - winSize.width / 2 + 30.f,
        - winSize.height / 2 + 30.f,
        this->m_pButtonMenu
    );

    this->reloadList();

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    return true;
}

void BackupViewLayer::reloadList() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (this->m_pList)
        this->m_pList->removeFromParent();

    BackupListView* list = nullptr;

    if (!LevelBackupManager::get()->levelHasBackupSettings(this->m_pLevel)) {
        m_pListLabel->setVisible(true);
    } else {
        m_pListLabel->setVisible(false);

        list = BackupListView::create(
            LevelBackupManager::get()->getBackupsForLevel(this->m_pLevel),
            this,
            356.f, 220.f
        );
    }

    this->m_pList = GJListLayer::create(
        list, this->m_sTitle.c_str(), { 0, 0, 0, 180 }, 356.0f, 220.0f
    );
    this->m_pList->setPosition(
        winSize / 2 - this->m_pList->getScaledContentSize() / 2
    );
    this->addChild(this->m_pList);
}

void BackupViewLayer::keyDown(enumKeyCodes key) {
	switch (key) {
        case KEY_Escape:
		    this->onExit(nullptr);
            break;
	}
}

void BackupViewLayer::onNew(CCObject*) {
    LevelBackupManager::get()->createBackupForLevel(this->m_pLevel);
    this->reloadList();
}

void BackupViewLayer::onSchedule(CCObject*) {
    BackupScheduleLayer::create(this->m_pLevel)->show();
}

void BackupViewLayer::onExit(CCObject*) {
    EditLevelLayer::scene(this->m_pLevel);
}

BackupViewLayer* BackupViewLayer::create(GJGameLevel* level) {
    auto ret = new BackupViewLayer;

    if (ret && ret->init(level)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

BackupViewLayer* BackupViewLayer::scene(GJGameLevel* level) {
    auto scene = CCScene::create();

    auto layer = BackupViewLayer::create(level);

    scene->addChild(layer);

    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(.5f, scene)
    );

    return layer;
}
