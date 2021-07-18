#pragma once

#include "LayerViewPopup.hpp"
#include <InputNode.hpp>

class LayerViewPopup;

class LayerEditPopup : public BrownAlertDelegate {
    protected:
        LayerManager::Layer* m_pELayer;
        InputNode* m_pNameInput;
        InputNode* m_pOpacityInput;
        LayerViewPopup* m_pViewPopup;

        void setup() override;
        
        void onClose(CCObject*) override;
        void onReset(CCObject*);

        friend class LayerViewPopup;
        
    public:
        static LayerEditPopup* create(LayerViewPopup*, LayerManager::Layer*);
};
