#pragma once

#include "Action.hpp"
#include <BrownAlertDelegate.hpp>
#include "ActionListView.hpp"

class UndoHistoryPopup : public BrownAlertDelegate {
    protected:
        ActionListView* m_pList;

        void setup() override;
    
    public:
        static UndoHistoryPopup* create();
};
