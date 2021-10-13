#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include "ThreeWayBoolSelect.hpp"

enum LocalLevelFilter {
    // 3-way-bools (yes, no, either)
    kLocalLevelFilterVerified,
    kLocalLevelFilterUploaded,
    kLocalLevelFilterCopied,
    kLocalLevelFilterSong,
    kLocalLevelFilter2Player,
    kLocalLevelFilterCoins,

    // ints
    kLocalLevelFilterPercentage,
    kLocalLevelFilterObjectCount,
    kLocalLevelFilterLength,
};

class LocalLevelFilterLayer : public BrownAlertDelegate {
    protected:
        LevelBrowserLayer* m_pBrowserLayer;
        std::unordered_map<LocalLevelFilter, ThreeWayBool> m_mStates;
        int m_nPercentageFilter;
        int m_nObjectCountFilter;
        int m_nLengthFilter;
        int m_nFilterCount = 0;

        void setup() override;
        void onToggle(ThreeWayBoolSelect*, ThreeWayBool);

        void addToggle(const char* text, LocalLevelFilter);
        void addInput(const char* text, LocalLevelFilter, int min, int max);

        void onSearch(CCObject*);

        ~LocalLevelFilterLayer();

    public:
        static LocalLevelFilterLayer* create(LevelBrowserLayer*);
};
