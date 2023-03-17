#pragma once

#include <cocos2d.h>

using namespace cocos2d;

class AutoGrowingLayout : public AxisLayout {
protected:
    float m_minLength;

    void apply(CCNode* on) override;

    AutoGrowingLayout(float minLength, Axis axis);

public:
    static AutoGrowingLayout* create(float minLength, Axis axis = Axis::Column);
};
