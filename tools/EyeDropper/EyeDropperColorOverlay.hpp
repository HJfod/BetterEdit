#pragma once

#include "eyeDropper.hpp"

class EyeDropperColorOverlay : public CCNode {
    protected:
        ColorChannelSprite* m_pSprite;

        bool init();
    
    public:
        static EyeDropperColorOverlay* create();
        static EyeDropperColorOverlay* addToCurrentScene();

        void setShowColor(ccColor3B const& color);
};
