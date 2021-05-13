#pragma once

#include "../offsets.hpp"
#include "../nodes/BrownAlertDelegate.hpp"

class ToolsLayer : public BrownAlertDelegate {
    protected:
        virtual void setup() override;

        void onRemapIDs(cocos2d::CCObject*);
        void onPasteString(cocos2d::CCObject*);
    
    public:
        static ToolsLayer* create();
};

