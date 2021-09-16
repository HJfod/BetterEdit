#pragma once

#include "EditorButton.hpp"
#include <unordered_set>

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

    inline bool operator==(ButtonGridItem const& item) const {
        return item.btn == this->btn;
    }
};

namespace std {
    template<>
    struct hash<ButtonGridItem> {
        inline std::size_t operator()(ButtonGridItem const& item) const {
            return as<std::size_t>(item.btn);
        }
    };
}

class ButtonGrid {
    protected:
        std::unordered_set<ButtonGridItem> m_vButtons;
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
