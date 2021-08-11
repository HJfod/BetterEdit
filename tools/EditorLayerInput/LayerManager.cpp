#include "LayerManager.hpp"

LayerManager* g_manager;

LayerManager::LevelName LayerManager::getLevelName() {
    if (!LevelEditorLayer::get()) {
        return "";
    }

    return LevelEditorLayer::get()->m_pLevelSettings->m_pLevel->levelName;
}

LayerManager::Layer* LayerManager::getLayer(int number) {
    if (number < 0) return nullptr;
    if (number > static_cast<int>(max_layer_num)) return nullptr;
    
    auto lvl = this->getLevel();

    if (!lvl) return nullptr;

    if (!lvl->m_mLayers[number])
        lvl->m_mLayers[number] = new Layer(number);

    return lvl->m_mLayers[number];
}

LayerManager::Level * LayerManager::getLevel() {
    auto lvl = this->getLevelName();

    if (!lvl.size())
        return nullptr;

    if (!m_mLevels.count(lvl))
        m_mLevels.insert({ lvl, new Level() });

    return m_mLevels[lvl];
}

void LayerManager::dataLoaded(DS_Dictionary* data) {}

void LayerManager::encodeDataTo(DS_Dictionary* data) {}

bool LayerManager::init() {
    return true;
}

LayerManager* LayerManager::get() {
    return g_manager;
}

bool LayerManager::initGlobal() {
    g_manager = new LayerManager();

    if (g_manager && g_manager->init()) {
        // g_manager->retain();
        return true;
    }

    CC_SAFE_DELETE(g_manager);
    return false;
}
