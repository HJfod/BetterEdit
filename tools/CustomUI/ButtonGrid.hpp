#pragma once

#include "EditorButton.hpp"

class ButtonGrid {
    protected:
        struct GridPosition {
            bool automatic = true;
            int x, y;

            inline GridPosition() {
                this->automatic = true;
            }
            inline GridPosition(int x, int y) {
                this->x = x;
                this->y = y;
            }
        };

        struct ButtonGridItem {
            EditorButton* btn;
            GridPosition pos;

            inline ButtonGridItem(EditorButton* b) {
                this->btn = b;
                this->pos = GridPosition();
            }
        };

        std::set<ButtonGridItem> m_vButtons;
        float m_fScale = 1.0f;
        CCSize m_obSize;

        ButtonGridItem getItem(EditorButton*);

    public:
        ~ButtonGrid();

        void addButton(EditorButton*);
        void removeButton(EditorButton*);

        void moveButton(EditorButton*, GridPosition const&);
        void swapButton(EditorButton*, EditorButton*);

        void updatePositions();

        void setSize(CCSize const&);
};
