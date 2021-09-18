#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class GroupSummaryPopup : public BrownAlertDelegate {
    protected:
        LevelEditorLayer* m_pEditor;
        std::vector<CCArray*> m_vPages;
        int m_nPage = 0;

        void setup() override;
        void onPage(CCObject*);

    public:
        static GroupSummaryPopup* create(LevelEditorLayer*);
};
