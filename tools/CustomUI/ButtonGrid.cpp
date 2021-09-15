#include "ButtonGrid.hpp"

ButtonGrid::~ButtonGrid() {
    for (auto const& v : this->m_vButtons)
        v.btn->release();
}

void ButtonGrid::addButton(EditorButton* b) {
    this->m_vButtons.push_back({ b });
    b->retain();
}

void ButtonGrid::removeButton(EditorButton* b) {
    for (auto const& v : this->m_vButtons) {
        if (v.btn == b) {
            this->m_vButtons.erase(v);
            v->release();
        }
    }
}

void ButtonGrid::moveButton(EditorButton* b, ButtonGrid::GridPosition const& p) {
    
}

void ButtonGrid::updatePositions() {

}
