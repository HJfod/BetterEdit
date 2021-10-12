#include "ImportListView.hpp"
#include "ImportLevelLayer.hpp"
#include "ImportLayer.hpp"

ImportLevelCell::ImportLevelCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

ImportLevelCell::~ImportLevelCell() {
    this->m_pObject->release();
}

void ImportLevelCell::draw() {
    reinterpret_cast<void(__thiscall*)(ImportLevelCell*)>(
        base + 0x59d40
    )(this);
}

void ImportLevelCell::loadFromObject(ImportObject* obj) {
    this->m_pObject = obj;
    this->m_pObject->retain();

    auto title = CCLabelBMFont::create(obj->m_pLevel->m_sLevelName.c_str(), "bigFont.fnt");
    title->setAnchorPoint({ .0f, .5f });
    title->setPosition(15.f, 38.f);
    title->setScale(.6f);
    this->addChild(title);

    auto creator = CCLabelBMFont::create(CCString::createWithFormat("by %s",
        obj->m_pLevel->m_sCreatorName.c_str()
    )->getCString(), "goldFont.fnt");
    creator->setAnchorPoint({ .0f, .5f });
    creator->setPosition(15.f, 25.f);
    creator->setScale(.4f);
    this->addChild(creator);

    auto pathLabel = CCLabelBMFont::create(obj->m_sFilePath.c_str(), "chatFont.fnt");
    pathLabel->setAnchorPoint({ .0f, .5f });
    pathLabel->setPosition(15.f, 12.f);
    pathLabel->setScale(.4f);
    this->addChild(pathLabel);

    auto menu = CCMenu::create();
    menu->setPosition(this->m_fWidth - 40.f, this->m_fHeight / 2);
    this->m_pLayer->addChild(menu);

    auto viewSpr = ButtonSprite::create(
        "View", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
    );
    viewSpr->setScale(.75f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(ImportLevelCell::onView)
    );
    viewBtn->setPosition(0, 0);
    menu->addChild(viewBtn);

    auto importToggle = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ImportLevelCell::onSelect), .75f
    );
    importToggle->setPosition(-45.f, 0);
    menu->addChild(importToggle);

    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
}

void ImportLevelCell::onDelete(CCObject*) {
    this->m_pList->getLayer()->removeItemFromList(this->m_pObject);
}

void ImportLevelCell::onView(CCObject*) {
    ImportLevelLayer::scene(this->m_pList, this->m_pObject);
}

void ImportLevelCell::onSelect(CCObject* pSender) {
    this->m_bSelected = !as<CCMenuItemToggler*>(pSender)->isToggled();
}

bool ImportLevelCell::isSelected() const {
    return this->m_bSelected;
}

ImportObject* ImportLevelCell::getObject() const {
    return this->m_pObject;
}

ImportLevelCell* ImportLevelCell::create(ImportListView* list, const char* key, CCSize size) {
    auto pRet = new ImportLevelCell(key, size);

    if (pRet) {
        pRet->m_pList = list;
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}



void ImportListView::setupList() {
    this->m_fItemSeparation = 50.0f;

    if (!this->m_pEntries->count()) return;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* ImportListView::getListCell(const char* key) {
    return ImportLevelCell::create(this, key, { this->m_fWidth, this->m_fItemSeparation });
}

void ImportListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<ImportLevelCell*>(cell)->loadFromObject(
        as<ImportObject*>(this->m_pEntries->objectAtIndex(index))
    );
    as<StatsCell*>(cell)->updateBGColor(index);
}

ImportListView* ImportListView::create(
    ImportLayer* layer,
    CCArray* objs,
    float width,
    float height
) {
    auto pRet = new ImportListView;

    if (pRet) {
        pRet->m_pLayer = layer;
        if (pRet->init(objs, kBoomListType_Import, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

ImportLayer* ImportListView::getLayer() const {
    return this->m_pLayer;
}
