#include "LayerManager.hpp"

LayerManager* g_manager;

LayerManager::LevelName LayerManager::getLevelName() {
    if (!LevelEditorLayer::get()) {
        return "";
    }

    return LevelEditorLayer::get()->m_pLevelSettings->m_pLevel->m_sLevelName;
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

void LayerManager::dataLoaded(DS_Dictionary* dict) {
    ITERATE_STEP_SUBDICT_NC(dict, lvl,
        auto level = (m_mLevels[lvl] = new Level());
        STEP_SUBDICT_NC(dict, "layers",
            ITERATE_STEP_SUBDICT_NC(dict, l,
                auto num = std::stoi(l);
                auto layer = (level->m_mLayers[num] = new Layer(num));
                layer->m_bLocked = dict->getBoolForKey("locked");
                layer->m_bVisible = dict->getBoolForKey("visible");
                layer->m_nOpacity = dict->getIntegerForKey("opacity");
                layer->m_sName = dict->getStringForKey("name");
            );
        );
    );
}

void LayerManager::encodeDataTo(DS_Dictionary* dict) {
    for (auto const& [name, lvl] : m_mLevels) {
        STEP_SUBDICT(dict, name.c_str(),
            STEP_SUBDICT(dict, "layers",
                for (auto const& layer : lvl->m_mLayers) {
                    if (layer) {
                        STEP_SUBDICT(dict, std::to_string(layer->m_nLayerNumber).c_str(),
                            dict->setBoolForKey("locked", layer->m_bLocked);
                            dict->setBoolForKey("visible", layer->m_bVisible);
                            dict->setIntegerForKey("opacity", layer->m_nOpacity);
                            if (layer->m_sName.size())
                                dict->setStringForKey("name", layer->m_sName);
                        );
                    }
                }
            );
        );
    }
}

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
