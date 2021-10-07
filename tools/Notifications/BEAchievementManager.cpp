#include "BEAchievementManager.hpp"

BEAchievementManager* g_manager;

bool BEAchievementManager::init() {
    return true;
}

bool BEAchievementManager::initGlobal() {
    if (!g_manager) {
        g_manager = new BEAchievementManager;
        if (g_manager && g_manager->init()) {
            return true;
        }
        CC_SAFE_DELETE(g_manager);
        return false;
    }
    return true;
}

BEAchievementManager* BEAchievementManager::get() {
    return g_manager;
}


