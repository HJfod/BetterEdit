#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include "LayerManager.hpp"

class LayerViewPopup : public BrownAlertDelegate {
    public:
        static constexpr const int page_max_item_count = 24;
        static constexpr const int page_row_item_count = 8;

    protected:
        int m_nCurrentPage;
        std::vector<CCNode*> m_vPageContent;
        CCLabelBMFont* m_pPageLabel;

        void setup() override;
        
        void onClose(CCObject*) override;
        void onPage(CCObject*);

        void updatePage();
        void createItemAtPosition(float x, float y, int index);
        
    public:
        static LayerViewPopup* create();
};