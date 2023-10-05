#include "BEShared.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>

std::vector<std::function<void()>> BESharedFuncs::editorExitEvents = {};

void BESharedFuncs::addEditorExitFunc(std::function<void()> func) {
    editorExitEvents.push_back(func);
}

// run on editor exit
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* level) {
        auto editor = LevelEditorLayer::get();
        if(editor != nullptr && m_sceneEnum == 3) {
            // run events
            for(auto& func : BESharedFuncs::editorExitEvents) {
                func();
            }
        }
        
        GameManager::returnToLastScene(level);
    }
};