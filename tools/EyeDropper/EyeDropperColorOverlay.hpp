#pragma once

#include "eyeDropper.hpp"

class EyeDropperColorOverlay : public CCNode {
    protected:
        ColorChannelSprite* m_pSprite;
        CCLabelBMFont* m_pLabel;
        CCScale9Sprite* m_pLabelBG;
        ColorSelectPopup* m_pTargetPopup;
        ColorAction* m_pTargetAction;
        bool* m_pStatusBool;

        bool init(ColorSelectPopup*);

        void onCancel(CCObject*);
    
    public:
        static EyeDropperColorOverlay* create(ColorSelectPopup*);
        static EyeDropperColorOverlay* addToCurrentScene(ColorSelectPopup*);

        void setShowColor(ccColor3B const& color);
        void setLabelText(bool inWindow);
        void setStatusBool(bool* statusBool);

        void finish(ccColor3B* color);
};
