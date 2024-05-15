#include <Geode/modify/EditLevelLayer.hpp>
#include "BackupListPopup.hpp"

using namespace geode::prelude;

class $modify(BackupLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level))
            return false;
        
        if (auto menu = this->getChildByID("folder-menu")) {
            auto backupsSpr = CCSprite::createWithSpriteFrameName("GJ_myServerBtn_001.png");
            backupsSpr->setScale(.65f);
            auto backupsBtn = CCMenuItemSpriteExtra::create(
                backupsSpr, this, menu_selector(BackupLevelLayer::onBackups)
            );
            backupsBtn->setID("backups-list"_spr);
            menu->addChild(backupsBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onBackups(CCObject*) {
        BackupListPopup::create(m_level)->show();
    }
};
