#include "BackupListView.hpp"

void BackupCell::loadFromBackup(LevelBackup* backup) {
    auto label = CCLabelBMFont::create(
        CCString::createWithFormat("%d", backup->objectCount)->getCString(),
        "bigFont.fnt"
    );

    label->setPosition(this->m_pLayer->getScaledContentSize() / 2);

    this->m_pLayer->addChild(label);

    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
}

void BackupCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(BackupCell*)>(
        base + 0x59d40
    )(this);
}

BackupCell::BackupCell(const char* name, CCSize size) :
	TableViewCell(name, size.width, size.height) {}

BackupCell* BackupCell::create(const char* key, CCSize size) {
    auto pRet = new BackupCell(key, size);

    if (pRet) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void BackupListView::setupList() {
    BoomListView::setupList();

    this->m_fItemSeparation = 55.0f;
}

TableViewCell* BackupListView::getListCell(const char* key) {
    return BackupCell::create(key, { this->m_fWidth, 55.0f });
}

void BackupListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<BackupCell*>(cell)->loadFromBackup(
        as<LevelBackup*>(this->m_pEntries->objectAtIndex(index))
    );
    as<StatsCell*>(cell)->updateBGColor(index);
}

BackupListView* BackupListView::create(CCArray* backups, float width, float height) {
    auto pRet = new BackupListView;

    if (pRet && pRet->init(backups, kBoomListType_Backup, width, height)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
