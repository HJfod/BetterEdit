#include "UIScaling.hpp"
#include <Geode/cocos/support/CCPointExtension.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/Geode.hpp>

CCPoint defaultAnchor(CCNode* node) {
    auto size = node->getParent()->getContentSize();
    CCPoint anchor = ccp(.5f, .5f);
    if (node->getPositionX() <= size.width * 0.25) {
        anchor.x = 0.f;
    }
    else if (node->getPositionX() >= size.width * 0.75) {
        anchor.x = 1.f;
    }
    if (node->getPositionY() <= size.height * 0.25) {
        anchor.y = 0.f;
    }
    else if (node->getPositionY() >= size.height * 0.75) {
        anchor.y = 1.f;
    }
    return anchor;
}

void scaleComponent(CCNode* node, float scale, std::optional<CCPoint> const& anchor) {
    if (fabsf(scale - 1.f) < 0.0001f) return;
    auto offsetAnchor = anchor.value_or(defaultAnchor(node)) - node->getAnchorPoint();
    auto sizeBefore = node->getScaledContentSize();
    node->setScale(scale);
    auto sizeAfter = node->getScaledContentSize();
    node->setPosition(node->getPosition() + (sizeBefore - sizeAfter) * offsetAnchor);
}

void scaleChild(CCNode* node, std::string const& id, float scale, std::optional<CCPoint> const& anchor) {
    if (auto child = node->getChildByID(id)) {
        scaleComponent(child, scale, anchor);
    }
}
