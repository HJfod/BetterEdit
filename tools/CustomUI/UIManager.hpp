#pragma once

#include "../../BetterEdit.hpp"
#include "CustomizeOverlay.hpp"

class UIManager {
    protected:
        CustomizeOverlay* m_pOverlay = nullptr;

        bool init();

    public:
        void dataLoaded(DS_Dictionary*);
        void encodeDataTo(DS_Dictionary*);

        bool isCustomizing();
        void stopCustomizing();
        void startCustomizing();

        static bool initGlobal();
        static UIManager* get();
};
