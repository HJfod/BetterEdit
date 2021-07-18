#include "LayerManager.hpp"

LayerManager* g_manager;

LayerManager::LevelName LayerManager::getLevel() {
    if (!LevelEditorLayer::get()) {
        return "";
    }

    return LevelEditorLayer::get()->m_pLevelSettings->m_level->levelName;
}

LayerManager::Layer* LayerManager::getLayer(int number) {
    if (number < 0) return nullptr;
    if (number > static_cast<int>(max_layer_num)) return nullptr;

    auto lvl = this->getLevel();

    if (!lvl.size())
        return nullptr;

    if (!m_mLayers[lvl][number])
        m_mLayers[lvl][number] = new Layer(number);

    return m_mLayers[lvl][number];
}

LayerManager::Layers LayerManager::getLayers() {
    auto lvl = this->getLevel();

    if (!lvl.size())
        return {};

    return m_mLayers[lvl];
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
