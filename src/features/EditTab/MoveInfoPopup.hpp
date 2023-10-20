#pragma once

#include <Geode/Geode.hpp>
#include "other/BrownAlertDelegate.hpp"

using namespace geode::prelude;

class MoveInfoPopup : public BrownAlertDelegate {
    protected:
        void setup() override;

    public:
        static MoveInfoPopup* create();
};
