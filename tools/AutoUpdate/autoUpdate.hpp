#pragma once

#include "../../BetterEdit.hpp"

class UpdateChecker : public CCObject {
    public:
        void onCheckLatestRelease(CCHttpClient*, CCHttpResponse*);
};

void checkForUpdates();
