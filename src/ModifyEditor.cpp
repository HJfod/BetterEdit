#include <ModifyEditor.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <optional>

using namespace geode::prelude;
using namespace better_edit;

GridChangeEvent::GridChangeEvent(float size, bool locked)
  : newSize(size), locked(locked) {}

EditorGrid* EditorGrid::get() {
    static auto inst = new EditorGrid();
    return inst;
}

void EditorGrid::zoom(bool in) {
    auto size = static_cast<float>(clamp(
        (pow(2, round(log2(m_size / 30.0))) * 30.0) * (in ? 0.5 : 2.0),
        30.0 / 8,
        30.0 * 4
    ));
    if (std::isnan(size)) {
        size = 30.f;
    }
    this->setSize(size);
}

float EditorGrid::getSize() const {
    return m_size;
}

void EditorGrid::setSize(float size) {
    m_size = size;
    GridChangeEvent(m_size, m_locked).post();
    EditorUI::get()->updateGridNodeSize();
}

bool EditorGrid::isLocked() const {
    return m_locked || m_size != 30.f;
}

void EditorGrid::setLocked(bool lock) {
    m_locked = lock;
    GridChangeEvent(m_size, m_locked).post();
}

class $modify(ObjectToolbox) {
    float gridNodeSizeForKey(int key) {
        return EditorGrid::get()->isLocked() ?
            EditorGrid::get()->getSize() : 
            ObjectToolbox::gridNodeSizeForKey(key);
    }
};

class $modify(EditorUI) {
    void updateGridNodeSize() {
        if (EditorGrid::get()->isLocked()) {
            auto mode = m_selectedMode;
            m_selectedMode = 2;
            EditorUI::updateGridNodeSize();
            m_selectedMode = mode;
        }
        else {
            EditorUI::updateGridNodeSize();
        }
    }
};
