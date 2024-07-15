#include "Editor.hpp"

std::vector<GameObject*> getSelectedObjects(EditorUI* ui) {
    if (ui->m_selectedObject) {
        return std::vector<GameObject*>({ ui->m_selectedObject });
    }
    else {
        return ccArrayToVector<GameObject*>(ui->m_selectedObjects);
    }
}
