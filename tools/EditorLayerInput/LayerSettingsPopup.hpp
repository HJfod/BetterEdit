#pragma once

#include "LayerViewPopup.hpp"
#include <InputNode.hpp>

class LayerViewPopup;

class LayerSettingsPopup : public BrownAlertDelegate {
    protected:
        Slider* m_pOpacitySlider;
        CCLabelBMFont* m_pOpacityLabel;
        LayerViewPopup* m_pViewPopup;

        void setup() override;
        
        void onClose(CCObject*) override;
        void onReset(CCObject*);
        void sliderChanged(CCObject*);

        friend class LayerViewPopup;
        
    public:
        static LayerSettingsPopup* create(LayerViewPopup*);
};
