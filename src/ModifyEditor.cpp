#include <ModifyEditor.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <optional>
#include <other/Utils.hpp>

using namespace geode::prelude;
using namespace better_edit;

GridChangeEvent::GridChangeEvent(float size, bool locked)
  : newSize(size), locked(locked) {}

EditorGrid* EditorGrid::get() {
    static auto inst = new EditorGrid();
    return inst;
}

void EditorGrid::zoom(bool in) {
    auto rounded = (pow(2, round(log2(m_size / 30.0))) * 30.0);
    auto size = clamp(static_cast<float>(rounded) * (in ? .5f : 2.f), 30.f / 8, 30.f * 4);
    if (!std::isnan(size)) {
        this->setSize(size);
    }
}

float EditorGrid::getSize() const {
    return m_size;
}

void EditorGrid::setSize(float size) {
    m_size = size;
    GridChangeEvent(m_size, m_locked).post();
    if (Mod::get()->template getSettingValue<bool>("auto-show-grid")) {
        GameManager::get()->setGameVariable("0038", true);
        LevelEditorLayer::get()->updateOptions();
    }
    EditorUI::get()->updateGridNodeSize();
}

bool EditorGrid::isDynamicallyLocked() const {
    return !m_locked && this->isLocked();
}

bool EditorGrid::isLocked() const {
    return m_locked || m_size != 30.f;
}

void EditorGrid::setLocked(bool lock) {
    m_locked = lock;
    GridChangeEvent(m_size, m_locked).post();
    if (Mod::get()->template getSettingValue<bool>("auto-show-grid")) {
        GameManager::get()->setGameVariable("0038", true);
        LevelEditorLayer::get()->updateOptions();
    }
}

class $modify(OTB, ObjectToolbox) {
    bool bypassLock = false;

    float gridNodeSizeForKey(int key) {
        if (EditorGrid::get()->isLocked() && !m_fields->bypassLock) {
            return EditorGrid::get()->getSize(); 
        }
        else {
            return ObjectToolbox::gridNodeSizeForKey(key);
        }
    }
};

class $modify(EditorUI) {
    // todo: persistent icon warning
    // void onCreateButton(CCObject* item) {
    //     EditorUI::onCreateButton(item);
    //     if (EditorGrid::get()->isLocked()) {
    //         auto otb = static_cast<OTB*>(ObjectToolbox::sharedState());
    //         otb->m_fields->bypassLock = true;
    //         if (otb->gridNodeSizeForKey(
    //             static_cast<CreateMenuItem*>(item)->m_objectID
    //         ) != 30.f) {
    //             showEditorWarning("Grid size is locked");
    //         }
    //         otb->m_fields->bypassLock = false;
    //     }
    // }

    void updateGridNodeSize() {
        if (EditorGrid::get()->isLocked()) {
            m_gridSize = EditorGrid::get()->getSize();
            m_editorLayer->m_drawGridLayer->m_activeGridNodeSize = m_gridSize;
        }
        else {
            EditorUI::updateGridNodeSize();
        }
    }
};
