#pragma once

#include "../../BetterEdit.hpp"
#include "editorLayerInput.hpp"
#include <BrownAlertDelegate.hpp>
#include "LayerManager.hpp"

class LayerEditNode;

class LayerViewPopup : public BrownAlertDelegate {
    public:
        static constexpr const int grid_max_item_count = 36;
        static constexpr const int grid_row_item_count = 6;

        static constexpr const int list_max_item_count = 7;

        static constexpr const int grid_tag_offset = 1000;
        static constexpr const int lock_tag_offset = 2000;
        static constexpr const int name_tag_offset = 4000;
        static constexpr const int view_tag_offset = 6000;
        static constexpr const int eye_tag_offset  = 7000;

        static constexpr const float s_fItemWidth = 250.0f;

        enum ViewType {
            kViewTypeGrid,
            kViewTypeList,
        };

        enum GridView {
            kGridViewNormal,
            kGridViewLocked,
            kGridViewVisible,
        };

    protected:
        int m_nCurrentPage;
        ViewType m_nViewType;
        GridView m_nGridView;
        bool m_bHideEmptyLayers;
        std::vector<CCMenuItemSpriteExtra*> m_vGridViewBtns;
        std::vector<CCNode*> m_vPageContent;
        std::map<int, int> m_mObjectsPerLayer;
        CCLabelBMFont* m_pPageLabel;
        ButtonSprite* m_pViewGridBtn;
        ButtonSprite* m_pViewListBtn;
        ButtonSprite* m_pViewEmptyBtn;

        void setup() override;
        
        void onClose(CCObject*) override;
        void onPage(CCObject*);
        void onLock(CCObject*);
        void onLockAll(CCObject*);
        void onSettings(CCObject*);
        void onShow(CCObject*);
        void onShowAll(CCObject*);
        void onGoToLayer(CCObject*);
        void onViewLayer(CCObject*);
        void onToggler(CCObject*);
        void onToggleEmpty(CCObject*);
        void onGlobalSettings(CCObject*);
        void onViewType(CCObject*);
        void onGridView(CCObject*);
        void onEditLayer(CCObject*);

        int maxItemsPerPage();
        void countObjectsPerLayer();
        int getObjectsInLayer(int);
        int countPages();
        int firstItemInPage();
        void updatePage();
        void createListItemAtPosition(float x, float y, bool color, int index);
        void createGridItemAtPosition(float x, float y, bool color, int index);
        void updateLayerItem(int index, LayerManager::Layer*);

        friend class LayerEditNode;
        
    public:
        static LayerViewPopup* create();
};
