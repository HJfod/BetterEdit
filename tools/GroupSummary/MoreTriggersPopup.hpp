#pragma once

#include "GroupSummaryPopup.hpp"

class MoreTriggersPopup : public ContextPopup {
    protected:
        GroupSummaryPopup* m_pPopup;
        int m_nGroup;

        void setup() override;

        friend class GroupSummaryPopup;
    
    public:
        static MoreTriggersPopup* create(GroupSummaryPopup*, CCNode*);
};
