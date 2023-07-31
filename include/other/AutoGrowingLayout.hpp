#pragma once

#include <cocos2d.h>

namespace better_edit {
    class AutoGrowingLayout : public cocos2d::AxisLayout {
    protected:
        float m_minLength;

        void apply(cocos2d::CCNode* on) override;

        AutoGrowingLayout(float minLength, cocos2d::Axis axis);

    public:
        static AutoGrowingLayout* create(float minLength, cocos2d::Axis axis = cocos2d::Axis::Column);
    };
}
