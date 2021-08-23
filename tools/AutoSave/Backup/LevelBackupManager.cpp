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
            auto arr = as<LevelBackupSettings*>(obj->getObject());

            STEP_SUBDICT(dict, obj->getStrKey(),
                dict->setBoolForKey("autosave", arr->m_bAutoBackup);
                dict->setIntegerForKey("autosave-rate", arr->m_nBackupEvery);
                dict->setIntegerForKey("autosave-last", arr->m_nLastBackupObjectCount);

                STEP_SUBDICT(dict, "backups", 
                    CCARRAY_FOREACH_B_BASE(arr->m_pBackups, backup, LevelBackup*, ix) {
                        STEP_SUBDICT(dict, CCString::createWithFormat("k%i", ix)->getCString(),
                            backup->save(dict);
                        );
                    }
                );
            );
        }
    );
}

void LevelBackupManager::dataLoaded(DS_Dictionary* dict) {
    STEP_SUBDICT_NC(dict, "levels",
        for (auto key : dict->getAllKeys()) {
            STEP_SUBDICT_NC(dict, key.c_str(),
                auto arr = LevelBackupSettings::create(nullptr);
                m_pLevels->setObject(arr, key.c_str());

                arr->m_bAutoBackup = dict->getBoolForKey("autosave");
                arr->m_nBackupEvery = dict->getIntegerForKey("autosave-rate");
                arr->m_nLastBackupObjectCount = dict->getIntegerForKey("autosave-last");

                if (arr->m_nBackupEvery < 1)
                    arr->m_nBackupEvery = arr->s_defaultBackupEvery;

                for (auto bkey : dict->getAllKeys()) {
                    if (bkey.starts_with('k')) {
                        STEP_SUBDICT_NC(dict, bkey.c_str(),
                            arr->m_pBackups->insertObject(new LevelBackup(dict), 0u);
                        );
                    }
                }
                STEP_SUBDICT_NC(dict, "backups",
                    for (auto bkey : dict->getAllKeys()) {
                        STEP_SUBDICT_NC(dict, bkey.c_str(),
                            arr->m_pBackups->insertObject(new LevelBackup(dict), 0u);
                        );
                    }
                );
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

bool LevelBackupManager::levelHasBackupSettings(GJGameLevel* level) {
    return m_pLevels->objectForKey(level->levelName);
}

bool LevelBackupManager::levelHasBackups(GJGameLevel* level) {
    if (!levelHasBackupSettings(level))
        return false;
    
    return getBackupSettingsForLevel(level)->m_pBackups->count();
}

LevelBackupSettings* LevelBackupManager::getBackupSettingsForLevel(GJGameLevel* level, bool create) {
    if (levelHasBackupSettings(level))
        return as<LevelBackupSettings*>(m_pLevels->objectForKey(level->levelName));
    
    if (create) {
        auto obj = LevelBackupSettings::create(level);
        m_pLevels->setObject(obj, level->levelName);
        return obj;
    }

    return nullptr;
}

CCArray* LevelBackupManager::getBackupsForLevel(GJGameLevel* level) {
    auto b = getBackupSettingsForLevel(level);
    if (b) return b->m_pBackups;
    return nullptr;
}

void LevelBackupManager::createBackupForLevel(GJGameLevel* level) {
    LevelBackupSettings* arr;
    if (!levelHasBackupSettings(level)) {
        arr = LevelBackupSettings::create(level);
        m_pLevels->setObject(arr, level->levelName);
    } else {
        arr = as<LevelBackupSettings*>(m_pLevels->objectForKey(level->levelName));
    }

    arr->m_pBackups->insertObject(new LevelBackup(level, ""), 0u);

    this->save();
}

void LevelBackupManager::removeBackupForLevel(GJGameLevel* level, LevelBackup* backup) {
    if (!levelHasBackupSettings(level))
        return;

    as<LevelBackupSettings*>(m_pLevels->objectForKey(level->levelName))
        ->m_pBackups->removeObject(backup);

    this->save();
}

void LevelBackupManager::handleAutoBackupForLevel(GJGameLevel* level) {
    auto b = getBackupSettingsForLevel(level);

    if (!b) return;
    if (!b->m_bAutoBackup) return;

    auto added = level->objectCount - b->m_nLastBackupObjectCount;
    if (added > b->m_nBackupEvery) {
        this->createBackupForLevel(level);
        b->m_nLastBackupObjectCount = level->objectCount;
    }
}
