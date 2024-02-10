
#include <Geode/modify/EditorUI.hpp>
#include <utils/EditableBMLabelProxy.hpp>

using namespace geode::prelude;

class $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        m_currentLayerLabel = EditableBMLabelProxy::replace(
            m_currentLayerLabel, 40.f, "Z",
            [this](auto str) {
                try {
                    m_editorLayer->m_currentLayer = std::stoi(str);
                }
                catch(...) {
                    m_editorLayer->m_currentLayer = -1;
                }
            }
        );
        
        return true;
    }
};
