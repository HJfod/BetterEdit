#pragma once

#include "../../BetterEdit.hpp"

class ContextMenu : public cocos2d::CCLayerColor {
    protected:
        cocos2d::ccColor4B m_colBG = { 255, 255, 255, 200 };
        cocos2d::ccColor3B m_colText = { 0, 0, 0 };
        cocos2d::ccColor3B m_colGray = { 150, 150, 150 };

        bool init() override;

    public:
        static ContextMenu* create();
        static ContextMenu* get();

        void show(cocos2d::CCPoint const& pos);
        void hide();

        static void loadRightClick(HMODULE);
        static void unloadRightClick();
};
