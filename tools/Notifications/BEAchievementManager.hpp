#pragma once

#include "../../BetterEdit.hpp"

class BEAchievementManager {
    protected:
        bool init();

    public:
        static bool initGlobal();
        static BEAchievementManager* get();
};
