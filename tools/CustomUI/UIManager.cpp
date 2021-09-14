#include "UIManager.hpp"

UIManager* g_manager = nullptr;

bool UIManager::isCustomizing() {
    return m_pOverlay;
}

void UIManager::stopCustomizing() {
    if (m_pOverlay) {
        m_pOverlay->removeFromParent();
        m_pOverlay = nullptr;
    }
}

void UIManager::startCustomizing() {
    auto lel = LevelEditorLayer::get();
    if (lel && !m_pOverlay) {
        m_pOverlay = CustomizeOverlay::create(lel->m_pEditorUI);
        lel->addChild(o, 1000);
    }
}

void UIManager::dataLoaded(DS_Dictionary* dict) {}

void UIManager::encodeDataTo(DS_Dictionary* dict) {}

bool UIManager::init() {
    return true;
}

bool UIManager::initGlobal() {
    if (!g_manager) {
        g_manager = new UIManager;
        if (g_manager && g_manager->init()) {
            return true;
        }
        CC_SAFE_DELETE(g_manager);
        return false;
    }
    return true;
}

UIManager* UIManager::get() {
    return g_manager;
}
