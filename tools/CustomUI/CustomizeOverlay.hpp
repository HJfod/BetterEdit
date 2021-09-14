#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"

class CustomizeArea : public CCLayer, public SuperMouseDelegate {
    protected:
        CCArray* m_pItems;
        
        void draw() override;

        bool init(CCArray*);

        ~CustomizeArea();

    public:
        void updateSize();

        static CustomizeArea* create(CCArray*);
};

class CustomizeOverlay : public CCLayerColor {
    protected:
        EditorUI* m_pUI;
        CustomizeArea* m_pAreaSideBtns;

        bool init(EditorUI*);

    public:
        static CustomizeOverlay* create(EditorUI*);
};
