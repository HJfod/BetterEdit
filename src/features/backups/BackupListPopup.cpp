#include "BackupListPopup.hpp"
#include "Backup.hpp"

bool BackupListPopup::setup(GJGameLevel* level) {
    m_level = level;

    this->setTitle(fmt::format("Backups for {}", level->m_levelName));

    auto bg = CCLayerColor::create({ 130, 64, 32, 255 });
    bg->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(bg, Anchor::Center);

    m_scrollLayer = ScrollLayer::create({ 275, 190 });
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setGap(2)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
    );
    m_mainLayer->addChildAtPosition(m_scrollLayer, Anchor::Center, -m_scrollLayer->getContentSize() / 2);

    bg->setContentSize(m_scrollLayer->getContentSize() + ccp(10, 10));

    auto borders = ListBorders::create();
    borders->setContentSize(m_scrollLayer->getContentSize() + ccp(10, 10));
    m_mainLayer->addChildAtPosition(borders, Anchor::Center);

    auto scrollbar = Scrollbar::create(m_scrollLayer);
    m_mainLayer->addChildAtPosition(scrollbar, Anchor::Center, ccp(m_scrollLayer->getContentWidth() / 2 + 15, 0));

    m_statusLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_statusLabel->setScale(.5f);
    m_statusLabel->setOpacity(205);
    m_mainLayer->addChildAtPosition(m_statusLabel, Anchor::Center);

    auto createBackupSpr = CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png");
    createBackupSpr->setScale(.75f);
    auto createBackupBtn = CCMenuItemSpriteExtra::create(
        createBackupSpr, this, menu_selector(BackupListPopup::onNewBackup)
    );
    m_buttonMenu->addChildAtPosition(createBackupBtn, Anchor::BottomRight, ccp(-10, 10));

    m_updateListListener.bind([this](UpdateBackupListEvent* ev) {
        if (ev->closeList) {
            this->onClose(nullptr);
        }
        else {
            this->updateList();
        }
        return ListenerResult::Propagate;
    });
    this->updateList();

    this->addCorners(Corner::Brown, Corner::Gold);

    return true;
}
void BackupListPopup::updateList() {
    m_scrollLayer->m_contentLayer->removeAllChildren();
    m_statusLabel->setVisible(false);

    auto backups = Backup::load(m_level);
    if (backups.empty()) {
        m_statusLabel->setString("No Backups Found");
        m_statusLabel->setVisible(true);
    }
    else for (auto backup : backups) {
        m_scrollLayer->m_contentLayer->addChild(BackupItem::create(backup, m_level));
    }

    m_scrollLayer->m_contentLayer->updateLayout();
}

void BackupListPopup::onNewBackup(CCObject*) {
    auto res = Backup::create(m_level);
    if (!res) {
        FLAlertLayer::create(
            "Unable to Backup",
            fmt::format("Unable to create a backup: {}", res.unwrapErr()),
            "OK"
        )->show();
    }
    this->updateList();
}

BackupListPopup* BackupListPopup::create(GJGameLevel* level) {
    auto ret = new BackupListPopup();
    if (ret && ret->initAnchored(350, 270, level)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
