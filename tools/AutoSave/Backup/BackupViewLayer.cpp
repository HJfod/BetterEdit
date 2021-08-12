#include "BackupViewLayer.hpp"
#include "LevelBackupManager.hpp"

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
        nullptr
    );
    addBtn->setPosition(winSize.width / 2 - 40.0f, - winSize.height / 2 + 40.0f);
    this->m_pButtonMenu->addChild(addBtn);

    std::string name = "Backups for " + this->m_pLevel->levelName;

    this->m_pList = GJListLayer::create(
        nullptr, name.c_str(), { 0, 0, 0, 0xb6 }, 356.0f, 220.0f
    );
    this->m_pList->setPosition(
        winSize / 2 - this->m_pList->getScaledContentSize() / 2
    );
    this->addChild(this->m_pList);

    if (!LevelBackupManager::get()->levelHasBackup(this->m_pLevel)) {
        auto label = CCLabelBMFont::create("No backups found!", "bigFont.fnt");

        label->setPosition(winSize / 2);
        label->setScale(.6f);

        this->addChild(label);
    }

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    return true;
}

void BackupViewLayer::keyDown(enumKeyCodes key) {
	switch (key) {
        case KEY_Escape:
		    this->onExit(nullptr);
            break;
	}
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
