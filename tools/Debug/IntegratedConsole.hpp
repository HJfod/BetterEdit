#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class IntegratedConsole : public BrownAlertDelegate {
    protected:
        DebugListView* m_pList;

        void setup() override;

    public:
        static IntegratedConsole* create();
};
