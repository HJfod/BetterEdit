#pragma once

#include "LayerViewPopup.hpp"
#include <InputNode.hpp>

class LayerViewPopup;

CCSprite* updateBtnSprite(CCMenuItemSpriteExtra*, CCSprite*);
void updateLockSprite(CCMenuItemSpriteExtra*, LayerManager::Layer*);
void updateShowSprite(CCMenuItemSpriteExtra*, LayerManager::Layer*);

class LayerEditNode : public CCMenu, TextInputDelegate {
    protected:
        static constexpr const int s_nameInputTag = 1;
        static constexpr const int s_opacityInputTag = 2;

        LayerManager::Layer* m_pELayer;
        InputNode* m_pNameInput;
        Slider* m_pOpacitySlider;
        InputNode* m_pOpacityLabel;
        LayerViewPopup* m_pViewPopup;
        CCMenuItemSpriteExtra* m_pResetOpacityBtn;
        CCMenuItemSpriteExtra* m_pLockBtn;
        CCMenuItemSpriteExtra* m_pShowBtn;

        bool init(LayerViewPopup*, LayerManager::Layer*);

        void onReset(CCObject*);
        void onResetOpacity(CCObject*);
        void onLock(CCObject*);
        void onShow(CCObject*);
        void sliderChanged(CCObject*);

        void textChanged(CCTextInputNode*) override;

        friend class LayerViewPopup;
        
    public:
        void updateNode(LayerManager::Layer*);

        static LayerEditNode* create(LayerViewPopup*, LayerManager::Layer*);
};
