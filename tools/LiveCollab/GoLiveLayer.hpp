#include "LiveManager.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>
#include "ButtonSelector.hpp"

class GoLiveLayer : public BrownAlertDelegate {
    protected:
        InputNode* m_pIPInput;
        ButtonSelector* m_pSelector;

        void setup() override;
    
    public:
        static GoLiveLayer* create();
};
