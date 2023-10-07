#include "AutoGrowingLayout.hpp"
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

void AutoGrowingLayout::apply(CCNode* on) {
    float height = 0.f;
    bool first = true;
    for (auto& node : CCArrayExt<CCNode>(on->getChildren())) {
        if (!first) {
            height += m_gap;
        }
        first = false;
        height += node->getScaledContentSize().height;
    }
    if (height < m_minLength) {
        height = m_minLength;
    }
    if (m_axis == Axis::Column) {
        on->setContentSize({ on->getContentSize().width, height });
    }
    else {
        on->setContentSize({ height, on->getContentSize().height });
    }
    AxisLayout::apply(on);
}

AutoGrowingLayout::AutoGrowingLayout(float minLength, Axis axis)
  : m_minLength(minLength), AxisLayout(axis) {}

AutoGrowingLayout* AutoGrowingLayout::create(float minLength, Axis axis) {
    return new AutoGrowingLayout(minLength, axis);
}
