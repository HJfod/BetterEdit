#include "LiveManager.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>
#include "ButtonSelector.hpp"

class GoLiveLayer : public BrownAlertDelegate {
    protected:
        InputNode* m_pAddrInput;
        InputNode* m_pRoomInput;
        InputNode* m_pRoomPWInput;

        void setup() override;

        void onHost(cocos2d::CCObject*);
    
    public:
        static GoLiveLayer* create();
};
