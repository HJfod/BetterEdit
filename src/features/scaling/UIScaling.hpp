#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>

using namespace geode::prelude;

CCPoint defaultAnchor(CCNode* node);
void scaleComponent(CCNode* node, float scale, std::optional<CCPoint> const& anchor = std::nullopt);
void scaleChild(CCNode* node, std::string const& id, float scale, std::optional<CCPoint> const& anchor = std::nullopt);
