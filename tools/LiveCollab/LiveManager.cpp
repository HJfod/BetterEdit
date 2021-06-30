#include "LiveManager.hpp"

LiveManager* g_manager;

bool LiveManager::init() {
    return true;
}

LiveManager* LiveManager::sharedState() {
    return g_manager;
}

bool LiveManager::initGlobal() {
    g_manager = new LiveManager();

    if (g_manager && g_manager->init())
        return true;

    CC_SAFE_DELETE(g_manager);
    return false;
}
