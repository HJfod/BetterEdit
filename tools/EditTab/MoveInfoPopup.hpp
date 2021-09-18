#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class MoveInfoPopup : public BrownAlertDelegate {
    protected:
        void setup() override;

    public:
        static MoveInfoPopup* create();
};
