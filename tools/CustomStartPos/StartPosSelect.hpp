#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class StartPosSelect : public CCNode {
    protected:
        Slider* m_pSlider;

        bool init();

    public:
        static StartPosSelect* create();

        void loadStartPositions(GJGameLevel*, std::function<void()>);
};
