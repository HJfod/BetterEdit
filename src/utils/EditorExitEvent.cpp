#include "EditorExitEvent.hpp"
#include <Geode/modify/GameManager.hpp>

// run on editor exit
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* level) {
        auto editor = LevelEditorLayer::get();
        if (editor && m_sceneEnum == 3) {
            EditorExitEvent().post();
        }
        GameManager::returnToLastScene(level);
    }
};
