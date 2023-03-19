#pragma once

#include <cocos2d.h>

using namespace cocos2d;

class Clip : public CCClippingNode {
protected:
    CCLayerColor* m_stencil;

    bool init(bool invert);

    void updateClippingRect();

public:
    static Clip* create(bool invert = false);

    void setContentSize(CCSize const& size) override;
};
