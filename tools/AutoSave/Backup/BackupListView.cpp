#include "BackupListView.hpp"
#include "BackupEditLayer.hpp"

static std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buf[128];
    ctime_s(buf, sizeof buf, &t);
    return buf;
}

void BackupCell::loadFromBackup(LevelBackup* backup) {
    this->m_pBackup = backup;

    m_pTitle = CCLabelBMFont::create("", "bigFont.fnt");

    m_pTitle->setAnchorPoint({ 0.0f, 0.5f });
    m_pTitle->setPosition(20.0f, 40.0f);
    m_pTitle->setScale(.65f);

    this->m_pLayer->addChild(m_pTitle);

    this->updateTitle(this->m_pBackup->name.c_str());


    auto time = timePointAsString(backup->savetime);

    auto label = CCLabelBMFont::create(
        CCString::createWithFormat(
            "%d objects - %s", backup->objectCount, time.c_str()
        )->getCString(), "goldFont.fnt"
    );

    label->setAnchorPoint({ 0.0f, 0.5f });
    label->setPosition(20.0f, 15.0f);
    label->setScale(.5f);

    this->m_pLayer->addChild(label);

    
    auto menu = CCMenu::create();

    auto viewBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor<ButtonSprite*>()
            .fromNode(ButtonSprite::create(
                "View", 0x32, 0, "bigFont.fnt", "GJ_button_01.png", 30.0f, .6f
            ))
            .scale(.9f)
            .done(),
        this,
        menu_selector(BackupCell::onView)
    );
    viewBtn->setPosition(this->m_fWidth / 2 - 45.0f, 0.0f);
    menu->addChild(viewBtn);

    menu->setPosition(this->m_fWidth / 2, this->m_fHeight / 2);

    this->m_pLayer->addChild(menu);


    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
}

void BackupCell::updateTitle(const char* newTitle) {
    m_pTitle->setString(
        strlen(newTitle) ? newTitle : "Unnamed Backup"
    );
    m_pTitle->limitLabelWidth(this->getContentSize().width - 160.0f, .65f, .1f);
}

void BackupCell::onView(CCObject* pSender) {
    BackupEditLayer::create(
        this->m_pBackupLayer, this, this->m_pBackup
    )->show();
}

void BackupCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(BackupCell*)>(
        base + 0x59d40
    )(this);
}

BackupCell::BackupCell(const char* name, CCSize size) :
	TableViewCell(name, size.width, size.height) {}

BackupCell* BackupCell::create(BackupViewLayer* layer, const char* key, CCSize size) {
    auto pRet = new BackupCell(key, size);

    if (pRet) {
        pRet->m_pBackupLayer = layer;
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void BackupListView::setupList() {
    this->m_fItemSeparation = 55.0f;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* BackupListView::getListCell(const char* key) {
    return BackupCell::create(this->m_pBackupLayer, key, { this->m_fWidth, 55.0f });
}

void BackupListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<BackupCell*>(cell)->loadFromBackup(
        as<LevelBackup*>(this->m_pEntries->objectAtIndex(index))
    );
    as<StatsCell*>(cell)->updateBGColor(index);
}

BackupListView* BackupListView::create(
    CCArray* backups,
    BackupViewLayer* parent,
    float width,
    float height
) {
    auto pRet = new BackupListView;

    if (pRet) {
        pRet->m_pBackupLayer = parent;
        if (pRet->init(backups, kBoomListType_Backup, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
