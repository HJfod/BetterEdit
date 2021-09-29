#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class GroupSummaryPopup : public BrownAlertDelegate {
    protected:
        LevelEditorLayer* m_pEditor;
        std::vector<CCNode*> m_vPageContent;
        int m_nPage = 0;
        int m_nItemCount = 10;
        float m_fItemWidth = 400.f;

        void setup() override;
        void onPage(CCObject*);
        void updatePage();

        virtual ~GroupSummaryPopup();

    public:
        static GroupSummaryPopup* create(LevelEditorLayer*);
};
