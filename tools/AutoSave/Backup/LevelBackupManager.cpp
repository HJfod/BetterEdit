#include "LevelBackupManager.hpp"

LevelBackupManager* g_manager;

bool LevelBackupManager::init() {
    return true;
}

void LevelBackupManager::encodeDataTo(DS_Dictionary* data) {

}

void LevelBackupManager::dataLoaded(DS_Dictionary* data) {

}

bool LevelBackupManager::initGlobal() {
    g_manager = new LevelBackupManager;

    if (g_manager && g_manager->init())
        return true;

    CC_SAFE_DELETE(g_manager);
    return false;
}

LevelBackupManager* LevelBackupManager::get() {
    return g_manager;
}

bool LevelBackupManager::levelHasBackup(GJGameLevel* level) {
    return levelHasBackup(level->levelName);
}

bool LevelBackupManager::levelHasBackup(std::string const& level) {
    return m_mLevels.count(level);
}

LevelBackupManager::backup_list* LevelBackupManager::getBackupsForLevel(GJGameLevel* level) {
    return getBackupsForLevel(level->levelName);
}

LevelBackupManager::backup_list* LevelBackupManager::getBackupsForLevel(std::string const& level) {
    if (levelHasBackup(level))
        return &m_mLevels[level];
    
    return nullptr;
}

void LevelBackupManager::createBackupForLevel(GJGameLevel* level) {
    
}
