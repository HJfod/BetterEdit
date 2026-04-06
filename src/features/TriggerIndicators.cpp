#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/TriggerControlGameObject.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/ranges.hpp>
#include <features/supporters/Pro.hpp>
#include <utils/ObjectIDs.hpp>
#include <utils/Editor.hpp>
#include <numbers>

using namespace geode::prelude;

static ccColor3B getTriggerColor(EffectGameObject* trigger) {
    using namespace object_ids;
    switch (trigger->m_objectID) {
        case MOVE_TRIGGER: case AREA_MOVE_TRIGGER: case EDIT_AREA_MOVE_TRIGGER:
            return ccc3(255, 0, 255);

        case ROTATE_TRIGGER: case AREA_ROTATE_TRIGGER: case EDIT_AREA_ROTATE_TRIGGER:
            return ccc3(127, 127, 255);
        
        case SCALE_TRIGGER: case AREA_SCALE_TRIGGER: case EDIT_AREA_SCALE_TRIGGER:
            return ccc3(63, 191, 255);
        
        case PULSE_TRIGGER: case AREA_TINT_TRIGGER: case EDIT_AREA_TINT_TRIGGER:
            return ccc3(255, 255, 0);
        
        case ALPHA_TRIGGER: case AREA_FADE_TRIGGER: case EDIT_AREA_FADE_TRIGGER:
            return ccc3(0, 255, 255);
        
        case TOGGLE_TRIGGER: {
            if (trigger->m_activateGroup) {
                return ccc3(0, 255, 127);
            }
            return ccc3(255, 63, 63);
        }
        case SPAWN_TRIGGER:  return ccc3(35, 204, 127);
        case FOLLOW_TRIGGER: return ccc3(255, 127, 127);
        
        case ADVANCED_FOLLOW_TRIGGER: case EDIT_ADVANCED_FOLLOW_TRIGGER: case RE_TARGET_ADVANCED_FOLLOW_TRIGGER:
            return ccc3(204, 255, 199);
        
        case ANIMATE_TRIGGER:           return ccc3(255, 183, 0);
        case FOLLOW_PLAYER_Y_TRIGGER:   return ccc3(255, 255, 127);
        case CHANGE_BG_TRIGGER:         return ccc3(127, 255, 255);
        case CHANGE_GROUND_TRIGGER:     return ccc3(255, 255, 127);
        case CHANGE_MIDGROUND_TRIGGER:  return ccc3(255, 127, 255);
        case TOUCH_TRIGGER:             return ccc3(0, 137, 178);
        case COUNT_TRIGGER:             return ccc3(255, 183, 252);
        case INSTANT_COUNT_TRIGGER:     return ccc3(255, 135, 255);
        case PICKUP_TRIGGER:            return ccc3(255, 109, 0);
        case RANDOM_TRIGGER:            return ccc3(63, 127, 255);

        case STOP_TRIGGER: {
            switch (static_cast<TriggerControlGameObject*>(trigger)->m_customTriggerValue) {
                default:
                case GJActionCommand::Stop: return ccc3(163, 0, 86);
                case GJActionCommand::Pause: return ccc3(255, 63, 0);
                case GJActionCommand::Resume: return ccc3(63, 255, 0);
            }
        }

        case ADVANCED_RANDOM_TRIGGER: case SEQUENCE_TRIGGER:
            return ccc3(165, 94, 255);

        case SPAWN_PARTICLE_TRIGGER: return ccc3(150, 150, 150);
        case RESET_TRIGGER:          return ccc3(255, 100, 0);
        case CAMERA_ZOOM_TRIGGER:    return ccc3(127, 127, 255);
        case CAMERA_STATIC_TRIGGER:  return ccc3(255, 127, 127);
        case CAMERA_ROTATE_TRIGGER:  return ccc3(255, 245, 0);
        case CAMERA_EDGE_TRIGGER:    return ccc3(0, 199, 255);
        case CAMERA_MODE_TRIGGER:    return ccc3(191, 255, 0);
        case COLLISION_TRIGGER:      return ccc3(83, 66, 250);
        case TIME_WARP_TRIGGER:      return ccc3(153, 127, 153);
        case BG_SPEED_TRIGGER:       return ccc3(127, 255, 255);
        case ON_DEATH_TRIGGER:       return ccc3(204, 101, 101);
        case PLAYER_CONTROL_TRIGGER: return ccc3(101, 255, 255);

        case MIDGROUND_SETUP_TRIGGER: case MIDGROUND_SPEED_TRIGGER:
            return ccc3(255, 127, 255);

        case CAMERA_OFFSET_TRIGGER: case CAMERA_GP_OFFSET_TRIGGER:
            return ccc3(127, 255, 127);
        
        default:
            return ccWHITE;
    }
}

static bool usesDashedLine(be::SlotType type) {
    return type == be::SlotType::Center;
}

enum class TriggerIndicatorColors {
    None,
    SelectedOnly,
    All,
};
static TriggerIndicatorColors parseTriggerIndicatorColors(std::string const& str) {
    switch (hash(str)) {
        case hash("None"): return TriggerIndicatorColors::None;
        default:
        case hash("Selected Only"): return TriggerIndicatorColors::SelectedOnly;
        case hash("All"): return TriggerIndicatorColors::All;
    }
}

// This does not take rotation into account but I'm assuming `boundingBox` is 
// too expensive
// If I'm wrong I can just use that
static CCRect getObjectRect(GameObject* obj) {
    const auto pos = obj->getPosition();
    const auto size = obj->getScaledContentSize();
    const auto anchor = obj->getAnchorPoint();
    return CCRect(
        ccp(pos.x - size.width * anchor.x, pos.y - size.height * anchor.y),
        size
    );
}

struct IndicatorDrawOptions final {
    bool blockyLines;
    bool arrowHeads;
};
struct Line final {
    CCPoint from;
    CCPoint to;

    Line(CCPoint const& from, CCPoint const& to) : from(from), to(to) {}
    float getAngle() const {
        return (to - from).getAngle();
    }
};
struct LineBatch final {
    std::vector<std::tuple<Line, bool>> lines;
    std::vector<CCRect> rects;
    bool dashed;
    GLfloat lineThickness;
    ccColor4B color;
};

// Static so we can reuse the allocations between calls
static std::vector<LineBatch> LINES_TO_DRAW {};
static std::vector<std::pair<CCPoint, be::SlotType>> INDICATOR_NODES_TO_DRAW {};
static std::vector<Line> BATCHED_LINES {};

static void batchDashedLine(Line const& line, float length) {
    auto isAtEnd = +[](Line const& line, CCPoint const& pos) {
        return line.from.x == line.to.x ?
            fabsf(pos.y - line.from.y) >= fabsf(line.to.y - line.from.y) :
            fabsf(pos.x - line.from.x) >= fabsf(line.to.x - line.from.x);
    };
    auto angle = atan2f(line.to.y - line.from.y, line.to.x - line.from.x);
    auto add = ccp(length * cosf(angle), length * sinf(angle));
    auto pos = line.from;
    while (!isAtEnd(line, pos)) {
        if (isAtEnd(line, pos + add)) {
            BATCHED_LINES.emplace_back(pos, line.to);
            break;
        }
        else {
            BATCHED_LINES.emplace_back(pos, pos + add);
            pos += add * 2;
        }
    }
}
static void drawCachedIndicators(IndicatorDrawOptions const& options) {
    const auto pickWhichToDraw = [](Line const& line, bool dashed, bool arrowTicks) {
        if (dashed) {
            batchDashedLine(line, 10);
        }
        else {
            BATCHED_LINES.push_back(line);
        }
        if (arrowTicks) {
            const auto angle = line.getAngle();
            const auto tickLength = 10;
            const auto tickAngle = std::numbers::pi_v<float> * .75f;
            BATCHED_LINES.push_back(Line(line.to, line.to + CCPoint::forAngle(angle - tickAngle) * tickLength));
            BATCHED_LINES.push_back(Line(line.to, line.to + CCPoint::forAngle(angle + tickAngle) * tickLength));
        }
    };

    // Preserve previous blend func
    GLint previousGlSrc;
    GLint previousGlDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &previousGlSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &previousGlDst);

    // Turn off blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto const& batch : LINES_TO_DRAW) {
        // This preserves the capacity
        BATCHED_LINES.clear();

        glLineWidth(batch.lineThickness);
        ccDrawColor4B(batch.color);

        for (auto const& [line, targetIsTrigger] : batch.lines) {
            if (options.blockyLines) {
                constexpr float MIN_TRIGGER_PROT = 15;
                
                const float minProtFromTarget = targetIsTrigger ? MIN_TRIGGER_PROT : 0;
                const auto delt = line.to - line.from;

                // Check if we can just draw a snakey line
                if (delt.x > MIN_TRIGGER_PROT + minProtFromTarget) {
                    const auto midPoint1 = ccp(line.from.x + delt.x / 2, line.from.y);
                    const auto midPoint2 = ccp(midPoint1.x,              line.to.y);
                    pickWhichToDraw(Line(line.from, midPoint1), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint1, midPoint2), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint2, line.to),   batch.dashed, options.arrowHeads);
                }
                // Otherwise we gotta snake a lil more
                else {
                    const auto midPoint1 = ccp(line.from.x + MIN_TRIGGER_PROT,line.from.y);
                    const auto midPoint2 = ccp(midPoint1.x,                   line.to.y - delt.y / 2);
                    const auto midPoint3 = ccp(line.to.x - minProtFromTarget, midPoint2.y);
                    const auto midPoint4 = ccp(midPoint3.x,                   line.to.y);
                    pickWhichToDraw(Line(line.from, midPoint1), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint1, midPoint2), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint2, midPoint3), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint3, midPoint4), batch.dashed, false);
                    pickWhichToDraw(Line(midPoint4, line.to),   batch.dashed, options.arrowHeads);
                }
            }
            else {
                pickWhichToDraw(line, batch.dashed, options.arrowHeads);
            }
        }

        // Draw all lines in one go
        ccDrawLines(reinterpret_cast<CCPoint*>(BATCHED_LINES.data()), BATCHED_LINES.size() * 2);

        // Draw rectangles
        // No batch method for these but luckily we shouldn't be drawing a lot 
        // of rectangles anyway
        for (auto const& rect : batch.rects) {
            ccDrawRect(rect.origin, rect.origin + rect.size);
        }
    }
    for (auto const& node : INDICATOR_NODES_TO_DRAW) {
        // todo: different shapes
        ccDrawColor4B(0, 0, 0, 255);
        ccDrawFilledCircle(node.first, 4, 2 * std::numbers::pi_v<float>, 10);
        ccDrawColor4B(255, 255, 255, 255);
        ccDrawFilledCircle(node.first, 3, 2 * std::numbers::pi_v<float>, 10);
    }

    // Reset blending to whatever it was before
    glBlendFunc(previousGlSrc, previousGlDst);
}

static float getDistanceBetweenRectsSq(CCRect const& rect1, CCRect const& rect2) {
    // https://stackoverflow.com/questions/10347085/distance-between-two-rectangles
    if (rect1.intersectsRect(rect2)) {
        return 0;
    }

    CCRect mostLeft = rect1.origin.x < rect2.origin.x ? rect1 : rect2;
    CCRect mostRight = rect2.origin.x < rect1.origin.x ? rect1 : rect2;

    float xDifference = mostLeft.origin.x == mostRight.origin.x ? 0 : mostRight.origin.x - (mostLeft.origin.x + mostLeft.size.width);
    xDifference = std::max(0.f, xDifference);

    CCRect upper = rect1.origin.y < rect2.origin.y ? rect1 : rect2;
    CCRect lower = rect2.origin.y < rect1.origin.y ? rect1 : rect2;

    float yDifference = upper.origin.y == lower.origin.y ? 0 : lower.origin.y - (upper.origin.y + upper.size.height);
    yDifference = std::max(0.f, yDifference);

    return xDifference * xDifference + yDifference * yDifference;
}
static CCPoint intersectionAtRectOutline(CCRect const& rect, CCPoint const& lineOrigin) {
    // https://stackoverflow.com/questions/1585525/how-to-find-the-intersection-point-between-a-line-and-a-rectangle

    auto const minX = rect.getMinX();
    auto const minY = rect.getMinY();
    auto const maxX = rect.getMaxX();
    auto const maxY = rect.getMaxY();
    auto const midX = rect.getMidX();
	auto const midY = rect.getMidY();

	auto const m = (midY - lineOrigin.y) / (midX - lineOrigin.x);

	if (lineOrigin.x <= midX) { // check "left" side
		auto const minXy = m * (minX - lineOrigin.x) + lineOrigin.y;
		if (minY <= minXy && minXy <= maxY) {
			return ccp(minX, minXy);
        }
	}

	if (lineOrigin.x >= midX) { // check "right" side
		auto const maxXy = m * (maxX - lineOrigin.x) + lineOrigin.y;
		if (minY <= maxXy && maxXy <= maxY) {
			return ccp(maxX, maxXy);
        }
	}

	if (lineOrigin.y <= midY) { // check "top" side
		auto const minYx = (minY - lineOrigin.y) / m + lineOrigin.x;
		if (minX <= minYx && minYx <= maxX) {
			return ccp(minYx, minY);
        }
	}

	if (lineOrigin.y >= midY) { // check "bottom" side
		auto const maxYx = (maxY - lineOrigin.y) / m + lineOrigin.x;
		if (minX <= maxYx && maxYx <= maxX)
			return ccp(maxYx, maxY);
	}

	// edge case when finding midpoint intersection: m = 0/0 = NaN
	return ccp(midX, midY);
}

struct Cluster final {
    CCRect rect;
    bool selected;
    size_t objects = 1;
    bool merged = false;
};
static void cluster(std::vector<Cluster>& clustered) {
    constexpr float MINIMUM_DISTANCE = 10 * 10; // 1/3 of a block
    for (size_t i = 0; i < clustered.size(); i += 1) {
        auto& cluster = clustered[i];
        if (cluster.merged) continue;
        for (size_t j = 0; j < clustered.size(); j += 1) {
            if (i == j) continue;
            auto& uncluster = clustered[j];
            if (uncluster.merged) continue;
            if (getDistanceBetweenRectsSq(cluster.rect, uncluster.rect) < MINIMUM_DISTANCE) {
                const auto diffMinX = uncluster.rect.getMinX() - cluster.rect.getMinX();
                if (diffMinX < 0.f) {
                    cluster.rect.origin.x += diffMinX;
                    cluster.rect.size.width -= diffMinX;
                }
                const auto diffMinY = uncluster.rect.getMinY() - cluster.rect.getMinY();
                if (diffMinY < 0.f) {
                    cluster.rect.origin.y += diffMinY;
                    cluster.rect.size.height -= diffMinY;
                }
                cluster.rect.size.width += std::max(0.f, uncluster.rect.getMaxX() - cluster.rect.getMaxX());
                cluster.rect.size.height += std::max(0.f, uncluster.rect.getMaxY() - cluster.rect.getMaxY());
                cluster.selected |= uncluster.selected;
                cluster.objects += 1;
                uncluster.merged = true;
            }
        }
    }
    // Remove all merged clusters from the result
    ranges::remove(clustered, [](Cluster const& cluster) { return cluster.merged; });
}

struct IndicatorOptions final {
    bool showTriggerToTrigger;
    bool showTargets;
    bool showClusterOutlines;
    GLfloat lineThickness;
    GLubyte lineOpacity;
    TriggerIndicatorColors colors;
};

static bool shouldRender(
    bool triggerSelected, bool targetSelected,
    bool triggerVisible, bool targetVisible,
    bool renderTypeOfIndicator
) {
    // If the target object or the trigger is selected, always draw indicator
    if (triggerSelected || targetSelected) {
        return true;
    }
    // Otherwise "show all indicators" must be enabled and either the trigger or 
    // the target object must be visible
    if (renderTypeOfIndicator && (triggerVisible || targetVisible)) {
        return true;
    }
    return false;
}

static void recalculateIndicatorsForSlot(
    EffectGameObject* trigger, CCArray* objs,
    CCPoint const& slotPosition, bool dashedLine,
    IndicatorOptions const& options,
    ccColor4B lineColor, CCRect const& objLayerRect
) {
    if (!objs) return;

    const bool triggerVisible = objLayerRect.intersectsRect(getObjectRect(trigger));

    LINES_TO_DRAW.push_back(LineBatch {
        .lines = {},
        .rects = {},
        .dashed = dashedLine,
        .lineThickness = options.lineThickness,
        .color = lineColor,
    });
    std::vector<Cluster> clustered;

    for (unsigned int i = 0u; i < objs->count(); i += 1) {
        auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));

        // Draw indicators between triggers
        if (object_ids::isTriggerID(obj->m_objectID)) {
            if (!shouldRender(
                trigger->m_isSelected, obj->m_isSelected,
                triggerVisible, objLayerRect.intersectsRect(getObjectRect(obj)),
                options.showTriggerToTrigger
            )) {
                continue;
            }
            const auto effObj = static_cast<EffectGameObject*>(obj);
            LINES_TO_DRAW.back().lines.push_back(std::make_tuple(
                Line(slotPosition, be::getTriggerSlots(effObj).first.calculateSlotPosition(effObj, 0)),
                true
            ));
        }
        // Draw clusters later
        else if (options.showTargets || obj->m_isSelected || trigger->m_isSelected) {
            clustered.push_back(Cluster {
                .rect = getObjectRect(obj),
                .selected = obj->m_isSelected,
            });
        }
    }

    // Calculate clusters
    cluster(clustered);

    for (auto const& cluster : clustered) {
        // If any object in the cluster is selected or if the cluster is visible, 
        // draw indicator line
        if (shouldRender(
            trigger->m_isSelected, cluster.selected,
            triggerVisible, objLayerRect.intersectsRect(cluster.rect),
            options.showTargets
        )) {
            LINES_TO_DRAW.back().lines.push_back(std::make_tuple(
                Line(
                    slotPosition,
                    options.showClusterOutlines ?
                        intersectionAtRectOutline(cluster.rect, slotPosition) :
                        ccp(cluster.rect.getMidX(), cluster.rect.getMidY())
                ),
                false
            ));
            if (options.showClusterOutlines) {
                LINES_TO_DRAW.back().rects.push_back(cluster.rect);
            }
        }
    }
}

static void recalculateIndicators(DrawGridLayer* dgl, IndicatorOptions const& options) {
    LINES_TO_DRAW.clear();
    INDICATOR_NODES_TO_DRAW.clear();
    BATCHED_LINES.clear();

    // Deref anything we can beforehand
    // const auto ui = m_editorLayer->m_editorUI;
    const auto groupDict = dgl->m_editorLayer->m_groupDict;

    // Calculate where the visible part of the view is on the object layer 
    // so we can just check if the object's position is within that rect 
    // without needing to transform it to world space first
    const auto objLayerRect = CCRect(
        dgl->m_editorLayer->m_objectLayer->convertToNodeSpace(CCPointZero),
        CCDirector::get()->getWinSize() / dgl->m_editorLayer->m_objectLayer->getScale()
    );
    
    for (auto trigger : CCArrayExt<EffectGameObject*>(dgl->m_effectGameObjects)) {
        auto [inputSlots, outputSlots] = be::getTriggerSlots(trigger);
        auto const triggerColor = options.colors != TriggerIndicatorColors::None ? 
            std::optional(to4B(getTriggerColor(trigger), options.lineOpacity)) : 
            std::nullopt;
        
        auto const triggerVisible = shouldRender(
            trigger->m_isSelected, false,
            objLayerRect.intersectsRect(getObjectRect(trigger)), false,
            true
        );
    
        if (outputSlots.targetGroupID) {
            auto const slotPos = outputSlots.calculateSlotPosition(trigger, 0);
            recalculateIndicatorsForSlot(
                trigger, be::getObjectsFromGroupDict(groupDict, trigger->m_targetGroupID),
                slotPos,
                usesDashedLine(*outputSlots.targetGroupID),
                options,
                triggerColor.value_or(ccc4(0, 255, 0, options.lineOpacity)),
                objLayerRect
            );
            if (triggerVisible) {
                INDICATOR_NODES_TO_DRAW.push_back(std::make_pair(slotPos, *outputSlots.targetGroupID));
            }
        }
        if (outputSlots.centerGroupID) {
            auto const slotPos = outputSlots.calculateSlotPosition(trigger, 1);
            recalculateIndicatorsForSlot(
                trigger, be::getObjectsFromGroupDict(groupDict, trigger->m_centerGroupID),
                slotPos, usesDashedLine(*outputSlots.centerGroupID),
                options,
                triggerColor.value_or(ccc4(0, 255, 255, options.lineOpacity)),
                objLayerRect
            );
            if (triggerVisible) {
                INDICATOR_NODES_TO_DRAW.push_back(std::make_pair(slotPos, *outputSlots.centerGroupID));
            }
        }
        if (inputSlots.targetGroupID) {
            if (triggerVisible) {
                INDICATOR_NODES_TO_DRAW.push_back(std::make_pair(
                    inputSlots.calculateSlotPosition(trigger, 0),
                    *inputSlots.targetGroupID
                ));
            }
        }
    }
}

static bool checkTooManyObjects(LevelEditorLayer* lel) {
    int count = 0;
    for (auto group : CCArrayExt<CCInteger*>(lel->m_groupDict->allKeys())) {
        count += static_cast<CCArray*>(lel->m_groupDict->objectForKey(group->getValue()))->count();
    }
    return count > 1000;
}

class $modify(DrawGridLayer) {
    $override
    void draw() {
        DrawGridLayer::draw();

        if (!Mod::get()->getSettingValue<bool>("trigger-indicators")) {
            return;
        }

        #define GET_VIEW_TAB(id) Mod::get()->template getSavedValue<bool>(id)
        #define GET_SETTING(ty, id) Mod::get()->template getSettingValue<ty>(id)

        static bool tooManyObjects = true;
        const auto options = IndicatorOptions {
            .showTriggerToTrigger = GET_VIEW_TAB("trigger-indicators-trigger-to-trigger"),
            .showTargets = GET_VIEW_TAB("trigger-indicators-show-all"),
            .showClusterOutlines = GET_VIEW_TAB("trigger-indicators-cluster-outlines"),
            .lineThickness = static_cast<GLfloat>(GET_SETTING(float, "trigger-indicator-thickness")),
            .lineOpacity = static_cast<GLubyte>(GET_SETTING(float, "trigger-indicator-opacity") * 255),
            .colors = parseTriggerIndicatorColors(GET_SETTING(std::string, "trigger-indicator-colors")),
        };
        const auto drawOptions = IndicatorDrawOptions {
            .blockyLines = GET_VIEW_TAB("trigger-indicators-blocky"),
            .arrowHeads = GET_SETTING(bool, "trigger-indicator-tickheads"),
        };

        static size_t FRAMES_SINCE_LAST_UPDATE = 0;
        if (options.showTargets) {
            if (FRAMES_SINCE_LAST_UPDATE > 50) {
                FRAMES_SINCE_LAST_UPDATE = 0;
                tooManyObjects = checkTooManyObjects(m_editorLayer);
            }
            // Update indicators every frame if there's not too many objects, 
            // or if there are then update every 50 frames
            if (!tooManyObjects || FRAMES_SINCE_LAST_UPDATE == 0) {
                recalculateIndicators(this, options);
            }
            FRAMES_SINCE_LAST_UPDATE += 1;
        }
        // Update indicators every frame otherwise
        else {
            recalculateIndicators(this, options);
        }
        
        drawCachedIndicators(drawOptions);
    }
};
