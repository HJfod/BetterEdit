#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include "DebugListView.hpp"

class DebugListView;

class IntegratedConsole : public BrownAlertDelegate {
    protected:
        DebugListView* m_pList;

        void setup() override;

    public:
        static IntegratedConsole* create();
};
