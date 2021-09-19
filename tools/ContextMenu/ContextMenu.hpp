#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"

class ContextMenu : public CCLayerColor, public SuperMouseDelegate {
    protected:
        ccColor4B m_colBG = { 255, 255, 255, 200 };
        ccColor3B m_colText = { 0, 0, 0 };
        ccColor3B m_colGray = { 150, 150, 150 };

        bool init() override;

        void mouseDownOutsideSuper(MouseButton, CCPoint const&) override;

    public:
        static ContextMenu* create();
        static ContextMenu* get();

        void show(CCPoint const& pos);
        void hide();
};
