#pragma once

#include "../nodes/BrownAlertDelegate.hpp"

class RemapItem : public cocos2d::CCLayer {
    protected:
        bool init(float);
        
    public:
        static RemapItem* create(float);

        std::tuple<int, int, int> getValue();
};

class RemapLayer : public BrownAlertDelegate {
    protected:
        virtual void setup() override;

        void onApply(cocos2d::CCObject*);
        void onAdd(cocos2d::CCObject*);

        cocos2d::CCLayer* m_pRemapItems;

    public:
        static RemapLayer* create();

        ~RemapLayer();
};
