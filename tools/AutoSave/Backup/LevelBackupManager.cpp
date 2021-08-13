#include "LevelBackupManager.hpp"

LevelBackupManager* g_manager;

#define STEP_SUBDICT_NC(dict, key, ...)         \
    if (dict->stepIntoSubDictWithKey(key)) {    \
        __VA_ARGS__                             \
        dict->stepOutOfSubDict();               \
    }

#define STEP_SUBDICT(dict, key, ...)            \
    {                                           \
    if (!dict->stepIntoSubDictWithKey(key)) {   \
        dict->setSubDictForKey(key);            \
        if (!dict->stepIntoSubDictWithKey(key)) \
            return;                             \
    }                                           \
    __VA_ARGS__                                 \
    dict->stepOutOfSubDict();                   \
    }

bool LevelBackupManager::init() {
    this->m_sFileName = "BEBackupManager.dat";

    this->m_pLevels = CCDictionary::create();
    this->m_pLevels->retain();

    this->setup();

    return true;
}

void LevelBackupManager::encodeDataTo(DS_Dictionary* dict) {
    STEP_SUBDICT(dict, "levels",
        CCDictElement* obj;
        CCDICT_FOREACH(m_pLevels, obj) {
            auto arr = as<CCArray*>(obj->getObject());

            STEP_SUBDICT(dict, obj->getStrKey(),
                CCARRAY_FOREACH_B_BASE(arr, backup, LevelBackup*, ix) {
                    STEP_SUBDICT(dict, CCString::createWithFormat("k%i", ix)->getCString(),
                        backup->save(dict);
                    );
                }
            );
        }
    );
}

void LevelBackupManager::dataLoaded(DS_Dictionary* dict) {
    STEP_SUBDICT_NC(dict, "levels",
        for (auto key : dict->getAllKeys()) {
            STEP_SUBDICT_NC(dict, key.c_str(),
                auto arr = CCArray::create();
                m_pLevels->setObject(arr, key.c_str());

                for (auto bkey : dict->getAllKeys()) {
                    STEP_SUBDICT_NC(dict, bkey.c_str(),
                        arr->addObject(new LevelBackup(dict));
                    );
                }
            );
        }
    );
}

void LevelBackupManager::firstLoad() {}

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
    return m_pLevels->objectForKey(level);
}

CCArray* LevelBackupManager::getBackupsForLevel(GJGameLevel* level) {
    return getBackupsForLevel(level->levelName);
}

CCArray* LevelBackupManager::getBackupsForLevel(std::string const& level) {
    if (levelHasBackup(level))
        return as<CCArray*>(m_pLevels->objectForKey(level));
    
    return nullptr;
}

void LevelBackupManager::createBackupForLevel(GJGameLevel* level) {
    CCArray* arr;
    if (!levelHasBackup(level)) {
        arr = CCArray::create();
        m_pLevels->setObject(arr, level->levelName);
    } else {
        arr = as<CCArray*>(m_pLevels->objectForKey(level->levelName));
    }

    arr->addObject(new LevelBackup(level, ""));
}
