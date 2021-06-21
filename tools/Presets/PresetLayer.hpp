#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class PresetLayer : public BrownAlertDelegate {
    protected:
        void setup() override;
    
        void onCreate(cocos2d::CCObject* pSender);
        void onRemove(cocos2d::CCObject* pSender);
        
    public:
        static PresetLayer* create();
};
