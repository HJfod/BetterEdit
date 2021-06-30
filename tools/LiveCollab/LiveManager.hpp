#include "../../BetterEdit.hpp"

class LiveManager {
    protected:
        bool m_bIsLive = false;

        bool init();

    public:
        static bool initGlobal();
        static LiveManager* sharedState();

        void goLive();
};
