#pragma once

#include "../../BetterEdit.hpp"
#include "editorLayerInput.hpp"
#include <BrownAlertDelegate.hpp>
#include "LayerManager.hpp"

class LayerEditPopup;

class LayerViewPopup : public BrownAlertDelegate {
    public:
        static constexpr const int page_max_item_count = 12;
        static constexpr const int page_row_item_count = 6;
        static constexpr const int lock_tag_offset = 500;
        static constexpr const int name_tag_offset = 2500;
        static constexpr const int view_tag_offset = 4500;

    protected:
        int m_nCurrentPage;
        std::vector<CCNode*> m_vPageContent;
        std::map<int, int> m_mObjectsPerLayer;
        CCLabelBMFont* m_pPageLabel;

        void setup() override;
        
        void onClose(CCObject*) override;
        void onPage(CCObject*);
        void onLock(CCObject*);
        void onLockAll(CCObject*);
        void onSettings(CCObject*);
        void onShow(CCObject*);
        void onShowAll(CCObject*);
        void onGoToLayer(CCObject*);

        void countObjectsPerLayer();
        void updatePage();
        void createItemAtPosition(float x, float y, int index);
        void updateLockSprite(CCMenuItemSpriteExtra*, LayerManager::Layer*);
        void updateShowSprite(CCMenuItemSpriteExtra*, LayerManager::Layer*);
        void updateLayerItem(int index, LayerManager::Layer*);

        friend class LayerEditPopup;
        
    public:
        static LayerViewPopup* create();
};
