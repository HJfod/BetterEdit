#include "GmdSaveManager.hpp"
#include "gmd.hpp"

GmdSaveManager* g_manager;

bool GmdSaveManager::init() {
    return true;
}

bool GmdSaveManager::initGlobal() {
    if (!g_manager) {
        g_manager = new GmdSaveManager;
        if (!g_manager || !g_manager->init()) {
            return false;
        }
    }
    return true;
}

GmdSaveManager* GmdSaveManager::get() {
    return g_manager;
}

void GmdSaveManager::load(LocalLevelManager* llm) {
    BetterEdit::log() << kDebugTypeLoading << "Loading levels..." << log_end();
}

void GmdSaveManager::save(LocalLevelManager* llm) {
    BetterEdit::log() << kDebugTypeLoading << "Saving levels..." << log_end();

    std::cout << CCFileUtils::sharedFileUtils()->getWritablePath() << "\n";
    std::cout << this->m_sDirName << "\n";

    if (!std::filesystem::exists(this->m_sDirName)) {
        std::filesystem::create_directory(this->m_sDirName);
    }

    // CCARRAY_FOREACH_B_TYPE(llm->m_pLevels, lvl, GJGameLevel) {
        // auto file = GmdFile();
    // }
}

void GmdSaveManager::backupCCLocalLevels(DS_Dictionary* dict) {
    BetterEdit::log() << kDebugTypeGeneric << "Backing up CCLocalLevels..." << log_end();
    dict->saveRootSubDictToCompressedFile("CCLocalLevels_BEBackup.dat");
}

void GmdSaveManager::setDirName(std::string const& str) {
    this->m_sDirName = str;
}

std::string const& GmdSaveManager::getDirName() {
    return this->m_sDirName;
}
