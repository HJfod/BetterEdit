#include <ModifyEditor.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <optional>

using namespace geode::prelude;
using namespace better_edit;

static bool GRID_SIZE_LOCKED = false;
static float GRID_SIZE = 30.f;

void better_edit::zoomGridSize(bool in) {
    setGridSize(clamp(
        powf(2, roundf(log2f(GRID_SIZE / 30.f))) * 30.f * (in ? .5f : 2.f),
        30.f / 8,
        30.f * 4
    ));
}

bool better_edit::isGridSizeLocked() {
    return GRID_SIZE_LOCKED || GRID_SIZE != 30.f;
}

float better_edit::getGridSize() {
    return GRID_SIZE;
}

void better_edit::setGridSize(float size) {
    GRID_SIZE = size;
    EditorUI::get()->updateGridNodeSize();
}

void better_edit::setGridSizeLocked(bool lock) {
    GRID_SIZE_LOCKED = lock;
}

class $modify(ObjectToolbox) {
    float gridNodeSizeForKey(int key) {
        return isGridSizeLocked() ?
            GRID_SIZE : 
            ObjectToolbox::gridNodeSizeForKey(key);
    }
};

class $modify(EditorUI) {
    void updateGridNodeSize() {
        if (GRID_SIZE) {
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
