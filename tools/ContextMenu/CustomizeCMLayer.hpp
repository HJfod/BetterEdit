#pragma once

#include "ContextMenu.hpp"
#include <BrownAlertDelegate.hpp>

class CustomizeCMLayer :
    public CCLayer,
    public SuperKeyboardDelegate,
    public SuperMouseDelegate
{
    protected:
        CCSprite* m_pBG;
        CCMenu* m_pButtonMenu;
        CCLabelBMFont* m_pSelectedModeLabel;
        int m_nSelectedMode = 0;
        std::vector<ContextMenu::ContextType> m_vModes;
        CCMenuItem* m_pClickedBtn = nullptr;
        ContextMenuItem* m_pHoverItem = nullptr;
        ContextMenu* m_pContextMenu;

        bool init();
        CCMenuItem* itemUnderMouse(CCPoint const&);
        ContextMenuItem* contextItemUnderMouse(CCPoint const&);

        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool mouseUpSuper(MouseButton, CCPoint const&) override;
        void mouseMoveSuper(CCPoint const&) override;
        bool mouseScrollSuper(float, float) override;

        bool keyDownSuper(enumKeyCodes) override;
        bool keyUpSuper(enumKeyCodes) override;

        void updateLabels();

        void onSelectMode(CCObject*);

        virtual ~CustomizeCMLayer();

    public:
        static CustomizeCMLayer* create();

        void show();
        void hide();
};
