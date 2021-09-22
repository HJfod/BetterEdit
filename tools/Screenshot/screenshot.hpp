#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../CustomKeybinds/SuperKeyboardManager.hpp"
#include <BrownAlertDelegate.hpp>

class ScreenFlash : public CCLayerColor {
    protected:
        bool init(ccColor4B const& col, float time);
    
    public:
        static ScreenFlash* create(ccColor4B const& col, float time);
        static ScreenFlash* show(ccColor4B const& col, float time);
};

class ScreenShotPopup : public BrownAlertDelegate {
    protected:
        CCRenderTexture* m_pTexture;
        CCSprite* m_pShot;

        void setup() override;
        void onCopy(CCObject*);
        void onSave(CCObject*);
    
        virtual ~ScreenShotPopup();

    public:
        static ScreenShotPopup* create(CCRenderTexture*);
};

class ScreenShotOverlay :
    public CCNode,
    public SuperMouseDelegate,
    public SuperKeyboardDelegate
{
    protected:
        bool m_bSelecting = false;
        bool m_bMoving = false;
        bool m_bExtendedMode = false;
        bool m_bIncludeGrid = false;
        CCPoint m_obStartPos;
        CCPoint m_obEndPos;
        CCPoint m_obMoveLastPos;
        EditorUI* m_pUI;
        CCLabelBMFont* m_pQualityLabel;
        CCLabelBMFont* m_pExtendedInfoLabel;
        CCLabelBMFont* m_pInfoLabel;
        CCLabelBMFont* m_pOptionsLabel;
        CCLayer* m_pLayer;
        float m_fQuality = 1.f;

        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool mouseUpSuper(MouseButton, CCPoint const&) override;
        bool mouseScrollSuper(float y, float x) override;
        void mouseMoveSuper(CCPoint const&) override;

        bool keyDownSuper(enumKeyCodes) override;

        void screenshot();
        void updateLabels();
        void draw() override;
        bool init() override;

        virtual ~ScreenShotOverlay();

    public:
        static ScreenShotOverlay* create();

        void setEditorUI(EditorUI*);
};

void takeScreenshot(EditorUI*);
