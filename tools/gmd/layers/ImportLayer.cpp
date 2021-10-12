#include "ImportLayer.hpp"
#include "ImportListView.hpp"

ImportLayer* g_pIsOpen = nullptr;

bool ImportLayer::init(CCArray* arr) {
    if (!CCLayer::init())
        return false;

    g_pIsOpen = this;

    this->m_pImportLevels = arr;
    this->m_pImportLevels->retain();

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

    this->m_pInfoLabel = CCLabelBMFont::create("", "bigFont.fnt");
    this->m_pInfoLabel->setPosition(winSize / 2);
    this->m_pInfoLabel->setZOrder(99);
    this->m_pInfoLabel->setScale(.65f);
    this->addChild(this->m_pInfoLabel);

    this->m_pButtonMenu = CCMenu::create();
    this->m_pButtonMenu->setZOrder(100);
    this->addChild(this->m_pButtonMenu);

    auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ImportLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	this->m_pButtonMenu->addChild(backBtn);

    auto importSelectedBtnSpr = ButtonSprite::create(
        "Import Selected", 0, 0, "goldFont.fnt", "GJ_button_02.png", 0, .8f
    );
    importSelectedBtnSpr->setScale(.6f);

    auto importSelectedBtn = CCMenuItemSpriteExtra::create(
		importSelectedBtnSpr,
		this,
		menu_selector(ImportLayer::onImportSelected)
	);
	importSelectedBtn->setPosition(-65.f, - winSize.height / 2 + 30.0f);
	this->m_pButtonMenu->addChild(importSelectedBtn);

    auto deleteSelectedBtnSpr = ButtonSprite::create(
        "Delete Selected", 0, 0, "goldFont.fnt", "GJ_button_06.png", 0, .8f
    );
    deleteSelectedBtnSpr->setScale(.6f);

    auto deleteSelectedBtn = CCMenuItemSpriteExtra::create(
		deleteSelectedBtnSpr,
		this,
		menu_selector(ImportLayer::onDeleteSelected)
	);
	deleteSelectedBtn->setPosition(65.f, - winSize.height / 2 + 30.0f);
	this->m_pButtonMenu->addChild(deleteSelectedBtn);

    this->reloadList();

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    return true;
}

void ImportLayer::reloadList() {
    if (this->m_pList) {
        this->m_pList->removeFromParent();
        this->m_pList = nullptr;
    }

    this->m_pInfoLabel->setVisible(!this->m_pImportLevels->count());
    this->m_pInfoLabel->setString("No Levels to Import");

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pList = GJListLayer::create(
        ImportListView::create(this, this->m_pImportLevels, 356.0f, 220.0f),
        "Import Levels", { 0, 0, 0, 180 }, 356.0f, 220.0f
    );
    this->m_pList->setPosition(
        winSize / 2 - this->m_pList->getScaledContentSize() / 2
    );
    this->addChild(this->m_pList);
}

void ImportLayer::addItemsToList(CCArray* arr) {
    this->m_pImportLevels->addObjectsFromArray(arr);
    this->reloadList();
}

void ImportLayer::removeItemFromList(ImportObject* obj) {
    this->m_pImportLevels->removeObject(obj);
    this->reloadList();
}

void ImportLayer::onExit(CCObject*) {
    if (this->m_pImportLevels->count()) {
        // !note: might be too confusing.
        FLAlertLayer::create(
            this,
            "Warning",
            "Cancel", "Leave",
            "Exiting will cause levels not imported to "
            "be deleted, and you'll have to import them "
            "again. Continue?"
        )->show();
    } else {
        this->exitForReal();
    }
}

void ImportLayer::onImportSelected(CCObject*) {
    CCARRAY_FOREACH_B_BASE(
        this->m_pList->m_pListView->m_pTableView->m_pContentLayer->getChildren(),
        cell,
        ImportLevelCell*,
        ix
    ) {
        if (cell->isSelected()) {
            LocalLevelManager::sharedState()->m_pLevels->insertObject(
                cell->getObject()->m_pLevel, 0u
            );
            this->m_pImportLevels->removeObject(cell->getObject());
        }
    }
    this->reloadList();
}

void ImportLayer::onDeleteSelected(CCObject*) {
    CCARRAY_FOREACH_B_BASE(
        this->m_pList->m_pListView->m_pTableView->m_pContentLayer->getChildren(),
        cell,
        ImportLevelCell*,
        ix
    ) {
        if (cell->isSelected()) {
            this->m_pImportLevels->removeObject(cell->getObject());
        }
    }
    this->reloadList();
}

void ImportLayer::exitForReal() {
    // CCDirector::sharedDirector()->replaceScene(
    //     CCTransitionFade::create(.5f, MenuLayer::scene(false))
    // );
    LevelBrowserLayer::scene(GJSearchObject::create(kGJSearchTypeMyLevels));
}

void ImportLayer::FLAlert_Clicked(FLAlertLayer*, bool btn2) {
    if (btn2) {
        this->exitForReal();
    }
}

void ImportLayer::keyDown(enumKeyCodes key) {
	switch (key) {
        case KEY_Escape:
		    this->onExit(nullptr);
            break;
	}
}

ImportLayer::~ImportLayer() {
    CC_SAFE_RELEASE(this->m_pImportLevels);
    g_pIsOpen = nullptr;
}

ImportLayer* ImportLayer::create(CCArray* arr) {
    auto ret = new ImportLayer;

    if (ret && ret->init(arr)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

ImportLayer* ImportLayer::scene(CCArray* arr, bool transition) {
    auto scene = CCScene::create();

    auto layer = ImportLayer::create(arr);

    scene->addChild(layer);

    if (transition) {
        scene = CCTransitionFade::create(.5f, scene);
    }

    CCDirector::sharedDirector()->replaceScene(scene);

    return layer;
}

ImportLayer* ImportLayer::isOpen() {
    return g_pIsOpen;
}
