#include "ImportLevelLayer.hpp"
#include "ImportLayer.hpp"

bool ImportLevelLayer::init(ImportListView* list, ImportObject* obj) {
    if (!CCLayer::init())
        return false;
    
    this->m_pList = list;
    this->m_pObject = obj;
    this->m_pList->retain();
    this->m_pObject->retain();
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto sRight = CCDirector::sharedDirector()->getScreenRight();
    
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

    auto titleLabel = CCLabelBMFont::create(
        obj->m_pLevel->m_sLevelName.c_str(), "bigFont.fnt"
    );
    titleLabel->setPosition(winSize.width / 2, winSize.height - 50.f);
    this->addChild(titleLabel);

    auto creator = CCLabelBMFont::create(CCString::createWithFormat("by %s",
        obj->m_pLevel->m_sCreatorName.c_str()
    )->getCString(), "goldFont.fnt");
    creator->setPosition(winSize.width / 2, winSize.height - 80.f);
    creator->setScale(.6f);
    this->addChild(creator);

    this->m_pButtonMenu = CCMenu::create();
    this->addChild(this->m_pButtonMenu);

    auto importBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("BE_Import_File.png"),
		this,
		menu_selector(ImportLevelLayer::onImport)
	);
	importBtn->setPosition(-125.f, 0.f);
	this->m_pButtonMenu->addChild(importBtn);

    auto playBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
		this,
        nullptr
	);
	playBtn->setPosition(45.f, 0.f);
	this->m_pButtonMenu->addChild(playBtn);

    auto editBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png"),
		this,
        nullptr
	);
	editBtn->setPosition(-45.f, 0.f);
	this->m_pButtonMenu->addChild(editBtn);

    auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ImportLevelLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	this->m_pButtonMenu->addChild(backBtn);

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    return true;
}

ImportLevelLayer::~ImportLevelLayer() {
    this->m_pList->release();
    this->m_pObject->release();
}

void ImportLevelLayer::onExit(CCObject*) {
    CCDirector::sharedDirector()->popSceneWithTransition(.5f, kPopTransitionFade);
}

void ImportLevelLayer::onImport(CCObject*) {
    this->m_pList->getLayer()->reloadList();
    this->onExit(nullptr);
}

void ImportLevelLayer::keyDown(enumKeyCodes key) {
	switch (key) {
        case KEY_Escape:
		    this->onExit(nullptr);
            break;
	}
}

ImportLevelLayer* ImportLevelLayer::create(ImportListView* list, ImportObject* obj) {
    auto ret = new ImportLevelLayer;

    if (ret && ret->init(list, obj)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

ImportLevelLayer* ImportLevelLayer::scene(ImportListView* list, ImportObject* obj) {
    auto scene = CCScene::create();

    auto layer = ImportLevelLayer::create(list, obj);

    scene->addChild(layer);

    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(
        0.5f, scene
    ));

    return layer;
}
