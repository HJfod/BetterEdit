
#include <Geode/utils/cocos.hpp>
#include "Tracking.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/StartPosObject.hpp>
#include <Geode/binding/CustomizeObjectLayer.hpp>
#include <Geode/binding/HSVWidgetPopup.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/SelectFontLayer.hpp>
#include <Geode/loader/Log.hpp>
#include <other/Utils.hpp>

std::string toDiffString(Ref<GameObject> obj) {
    return fmt::format(
        "{}_{}_{}_{}_{}_{}_{}",
        obj->m_objectID,
        obj->getPosition().x,
        obj->getPosition().y,
        obj->getRotation(),
        obj->getScale(),
        obj->m_baseColorID,
        obj->m_detailColorID
    );
}

std::string toDiffString(CCPoint const& point) {
    return fmt::format("({},{})", point.x, point.y);
}

std::string toDiffString(ccHSVValue const& value) {
    return fmt::format(
        "({},{},{},{},{})",
        value.h, value.s, value.v,
        value.absoluteSaturation, value.absoluteBrightness
    );
}

std::string toDiffString(ccColor3B const& value) {
    return fmt::format("({},{},{})", value.r, value.g, value.b);
}

std::string toDiffString(ccColor4B const& value) {
    return fmt::format("({},{},{},{})", value.r, value.g, value.b, value.a);
}

std::string toDiffString(ColorState const& value) {
    return toDiffString(
        value.color, value.opacity, value.blending,
        value.playerColor, value.copyColorID, value.copyHSV
    );
}

std::string toDiffString(ObjColorState const& value) {
    return toDiffString(
        value.base, value.baseHSV,
        value.detail, value.detailHSV,
        value.glow
    );
}

std::string toDiffString(ObjState const& value) {
    return toDiffString(
        value.groups, value.editorLayer1, value.editorLayer2, value.zOrder, 
        value.zLayer, value.dontFade, value.dontEnter, value.groupParent,
        value.highDetail
    );
}

std::string toDiffString(LevelSettingsState const& value) {
    return toDiffString(
        value.speed, value.mode, value.flipGravity, value.mini,
        value.dual, value.twoPlayer, value.songOffset, value.fadeIn,
        value.fadeOut, value.bg, value.ground, value.groundLine, value.font
    );
}

std::string toDiffString(TriggerState const& value) {
    return std::visit(makeVisitor {
        [](TriggerState::Color const& color) {
            return toDiffString(
                color.channel,
                color.fadeTime,
                color.color,
                color.opacity,
                color.blending,
                color.playerColor1,
                color.playerColor2,
                color.copyID,
                color.copyHSV,
                color.copyOpacity
            );
        },
        [](TriggerState::Move const& move) {
            return toDiffString(
                move.x,
                move.y,
                move.lockToPlayerX,
                move.lockToPlayerY,
                move.easing,
                move.easingRate,
                move.time,
                move.targetGroupID,
                move.useTarget,
                move.followGroupID,
                move.followType
            );
        },
        [](TriggerState::Stop const& stop) {
            return toDiffString(stop.targetGroupID);
        },
        [](TriggerState::Pulse const& pulse) {
            return toDiffString(
                pulse.targetID,
                pulse.targetGroup,
                pulse.mainOnly,
                pulse.detailOnly,
                pulse.color,
                pulse.pulseHSV,
                pulse.copyID,
                pulse.copyHSV,
                pulse.fadeIn,
                pulse.hold,
                pulse.fadeOut,
                pulse.exclusive
            );
        },
        [](TriggerState::Alpha const& alpha) {
            return toDiffString(
                alpha.targetGroupID,
                alpha.time,
                alpha.opacity
            );
        },
        [](TriggerState::Toggle const& toggle) {
            return toDiffString(
                toggle.targetGroupID,
                toggle.activateGroup
            );
        },
        [](TriggerState::Spawn const& spawn) {
            return toDiffString(
                spawn.targetGroupID,
                spawn.delayTime,
                spawn.editorDisable
            );
        },
        [](TriggerState::Rotate const& rotate) {
            return toDiffString(
                rotate.targetGroupID,
                rotate.centerGroupID,
                rotate.degrees,
                rotate.times360,
                rotate.lockObjectRotation,
                rotate.easing,
                rotate.easingRate,
                rotate.time
            );
        },
        [](TriggerState::Follow const& follow) {
            return toDiffString(
                follow.targetGroupID,
                follow.followGroupID,
                follow.xMod,
                follow.yMod,
                follow.time
            );
        },
        [](TriggerState::Shake const& shake) {
            return toDiffString(
                shake.strength, shake.interval, shake.duration
            );
        },
        [](TriggerState::Animation const& animation) {
            return toDiffString(
                animation.targetGroupID, animation.animationID
            );
        },
        [](TriggerState::FollowPlayerY const& followPlayerY) {
            return toDiffString(
                followPlayerY.targetGroupID,
                followPlayerY.speed,
                followPlayerY.delay,
                followPlayerY.offset,
                followPlayerY.maxSpeed,
                followPlayerY.time
            );
        },
        [](TriggerState::Touch const& touch) {
            return toDiffString(
                touch.targetGroupID,
                touch.dualMode,
                touch.holdMode,
                touch.toggleMode
            );
        },
        [](TriggerState::Count const& count) {
            return toDiffString(
                count.itemID,
                count.targetGroupID,
                count.targetCount,
                count.activateGroup,
                count.multiActivate
            );
        },
        [](TriggerState::InstantCount const& instantCount) {
            return toDiffString(
                instantCount.itemID,
                instantCount.targetGroupID,
                instantCount.targetCount,
                instantCount.activateGroup,
                instantCount.comparisonType
            );
        },
        [](TriggerState::Pickup const& pickup) {
            return toDiffString(pickup.itemID, pickup.count);
        },
        [](TriggerState::Collision const& collision) {
            return toDiffString(collision.blockAID, collision.blockBID);
        },
        [](TriggerState::OnDeath const& onDeath) {
            return toDiffString(onDeath.targetGroupID, onDeath.activateGroup);
        },
    }, value.props) + "," + toDiffString(value.touchTrigger, value.spawnTrigger, value.multiTrigger);
}

std::string toDiffString(std::vector<short> const& value) {
    std::string res = "[";
    for (auto& c : value) {
        res += std::to_string(c) + ",";
    }
    return res + "]";
}

std::string toDiffString(bool value) {
    return value ? "t" : "f";
}

std::string toDiffString(ZLayer const& value) {
    return std::to_string(static_cast<int>(value));
}

bool BlockAll::blocked() {
    return s_current;
}

BlockAll::BlockAll() {
    if (!s_current) {
        s_current = this;
    }
}

BlockAll::~BlockAll() {
    if (s_current == this) {
        s_current = nullptr;
    }
}

bool Detail::operator==(Detail const& other) const {
    return info == other.info && icon == other.icon;
}

ColorState ColorState::from(ColorAction* action) {
    return ColorState {
        .color = action->m_color,
        .opacity = action->m_opacity,
        .blending = action->m_blending,
        .playerColor = action->m_playerColor,
        .copyColorID = action->m_copyID,
        .copyHSV = action->m_copyHSV,
    };
}

void ColorState::to(ColorAction* action) const {
    action->m_color = color;
    action->m_opacity = opacity;
    action->m_blending = blending;
    action->m_playerColor = playerColor;
    action->m_copyID = copyColorID;
    action->m_copyHSV = copyHSV;
}

ObjColorState ObjColorState::from(GameObject* obj) {
    return ObjColorState {
        .base = obj->m_baseColor ? obj->m_baseColor->m_colorID : 0,
        .baseHSV = obj->m_baseColor ? obj->m_baseColor->m_hsv : ccHSVValue(),
        .detail = obj->m_detailColor ? obj->m_detailColor->m_colorID : 0,
        .detailHSV = obj->m_detailColor ? obj->m_detailColor->m_hsv : ccHSVValue(),
        .glow = !obj->m_isGlowDisabled,
    };
}

void ObjColorState::to(GameObject* obj) const {
    if (obj->m_baseColor) {
        obj->m_baseColor->m_colorID = base;
        obj->m_baseColor->m_hsv = baseHSV;
    }
    if (obj->m_detailColor) {
        obj->m_detailColor->m_colorID = detail;
        obj->m_detailColor->m_hsv = detailHSV;
    }
    obj->m_isGlowDisabled = !glow;
    obj->m_shouldUpdateColorSprite = true;
}

ObjState ObjState::from(GameObject* obj) {
    return ObjState {
        .groups = obj->getGroupIDs(),
        .editorLayer1 = obj->m_editorLayer,
        .editorLayer2 = obj->m_editorLayer2,
        .zOrder = obj->m_gameZOrder,
        .zLayer = obj->m_zLayer,
        .dontFade = obj->m_isDontFade,
        .dontEnter = obj->m_isDontEnter,
        .groupParent = obj->m_isGroupParent,
        .highDetail = obj->m_highDetail,
    };
}

void ObjState::to(GameObject* obj) const {
    auto _ = BlockAll();
    obj->m_groupCount = 0;
    for (auto& group : this->groups) {
        obj->addToGroup(group);
    }
    obj->m_editorLayer = this->editorLayer1;
    obj->m_editorLayer2 = this->editorLayer2;
    obj->m_gameZOrder = this->zOrder;
    obj->m_zLayer = this->zLayer;
    obj->m_isDontFade = this->dontFade;
    obj->m_isDontEnter = this->dontEnter;
    obj->m_isGroupParent = this->groupParent;
    obj->m_highDetail = this->highDetail;
}

LevelSettingsState LevelSettingsState::from(LevelSettingsObject* obj) {
    return LevelSettingsState {
        .speed = obj->m_startSpeed,
        .mode = obj->m_startMode,
        .flipGravity = obj->m_isFlipped,
        .mini = obj->m_startMini,
        .dual = obj->m_startDual,
        .twoPlayer = obj->m_twoPlayerMode,
        .songOffset = obj->m_songOffset,
        .fadeIn = obj->m_fadeIn,
        .fadeOut = obj->m_fadeOut,
        .bg = obj->m_backgroundIndex,
        .ground = obj->m_groundIndex,
        .groundLine = obj->m_groundLineIndex,
        .font = obj->m_fontIndex,
    };
}

void LevelSettingsState::to(LevelSettingsObject* obj) const {
    // needs to be updated first
    if (obj = LevelEditorLayer::get()->m_levelSettings) {
        LevelEditorLayer::get()->updateLevelFont(font);
    }
    obj->m_startSpeed = speed;
    obj->m_startMode = mode;
    obj->m_isFlipped = flipGravity;
    obj->m_startMini = mini;
    obj->m_startDual = dual;
    obj->m_twoPlayerMode = twoPlayer;
    obj->m_songOffset = songOffset;
    obj->m_fadeIn = fadeIn;
    obj->m_fadeOut = fadeOut;
    obj->m_backgroundIndex = bg;
    obj->m_groundIndex = ground;
    obj->m_groundLineIndex = groundLine;
    obj->m_fontIndex = font;
}

std::optional<TriggerState> TriggerState::from(EffectGameObject* obj) {
    switch (obj->m_objectID) {
        case 899: {
            return TriggerState {
                .props = {TriggerState::Color {
                    .channel = obj->m_targetColorID,
                    .fadeTime = obj->m_duration,
                    .color = obj->m_colColor,
                    .opacity = obj->m_opacity,
                    .blending = obj->m_blending,
                    .playerColor1 = obj->m_playerColor1,
                    .playerColor2 = obj->m_playerColor2,
                    .copyID = obj->m_copyColorID,
                    .copyHSV = obj->m_hsvValue,
                    .copyOpacity = obj->m_copyOpacity,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 901: {
            return TriggerState {
                .props = {TriggerState::Move {
                    .x = obj->m_move.x,
                    .y = obj->m_move.y,
                    .lockToPlayerX = obj->m_lockToPlayerX,
                    .lockToPlayerY = obj->m_lockToPlayerY,
                    .easing = obj->m_easingType,
                    .easingRate = obj->m_easingRate,
                    .time = obj->m_duration,
                    .targetGroupID = obj->m_targetGroupID,
                    .useTarget = obj->m_useTarget,
                    .followGroupID = obj->m_centerGroupID,
                    .followType = obj->m_moveTargetType,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1616: {
            return TriggerState {
                .props = {TriggerState::Stop {
                    .targetGroupID = obj->m_targetGroupID,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1006: {
            return TriggerState {
                .props = {TriggerState::Pulse {
                    .targetID = obj->m_targetGroupID,
                    .targetGroup = obj->m_pulseGroupMode == 1,
                    .mainOnly = obj->m_pulseMainOnly,
                    .detailOnly = obj->m_pulseDetailOnly,
                    .color = obj->m_colColor,
                    .pulseHSV = obj->m_pulseHSVMode == 1,
                    .copyID = obj->m_copyColorID,
                    .copyHSV = obj->m_hsvValue,
                    .fadeIn = obj->m_fadeInTime,
                    .hold = obj->m_holdTime,
                    .fadeOut = obj->m_fadeOutTime,
                    .exclusive = obj->m_pulseExclusive,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1007: {
            return TriggerState {
                .props = {TriggerState::Alpha {
                    .targetGroupID = obj->m_targetGroupID,
                    .time = obj->m_duration,
                    .opacity = obj->m_opacity,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1049: {
            return TriggerState {
                .props = {TriggerState::Toggle {
                    .targetGroupID = obj->m_targetGroupID,
                    .activateGroup = obj->m_activateGroup,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1268: {
            return TriggerState {
                .props = {TriggerState::Spawn {
                    .targetGroupID = obj->m_targetGroupID,
                    .delayTime = obj->m_spawnDelay,
                    .editorDisable = obj->m_editorDisabled,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1346: {
            return TriggerState {
                .props = {TriggerState::Rotate {
                    .targetGroupID = obj->m_targetGroupID,
                    .centerGroupID = obj->m_centerGroupID,
                    .degrees = obj->m_rotateDegrees,
                    .times360 = obj->m_times360,
                    .lockObjectRotation = obj->m_lockObjectRotation,
                    .easing = obj->m_easingType,
                    .easingRate = obj->m_easingRate,
                    .time = obj->m_duration,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1347: {
            return TriggerState {
                .props = {TriggerState::Follow {
                    .targetGroupID = obj->m_targetGroupID,
                    .followGroupID = obj->m_centerGroupID,
                    .xMod = obj->m_followMod.x,
                    .yMod = obj->m_followMod.y,
                    .time = obj->m_duration,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1520: {
            return TriggerState {
                .props = {TriggerState::Shake {
                    .strength = obj->m_shakeStrength,
                    .interval = obj->m_shakeInterval,
                    .duration = obj->m_duration,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1585: {
            return TriggerState {
                .props = {TriggerState::Animation {
                    .targetGroupID = obj->m_targetGroupID,
                    .animationID = obj->m_animationID,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1814: {
            return TriggerState {
                .props = {TriggerState::FollowPlayerY {
                    .targetGroupID = obj->m_targetGroupID,
                    .speed = obj->m_followYSpeed,
                    .delay = obj->m_followYDelay,
                    .offset = obj->m_followYOffset,
                    .maxSpeed = obj->m_followYMaxSpeed,
                    .time = obj->m_duration,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1595: {
            return TriggerState {
                .props = {TriggerState::Touch {
                    .targetGroupID = obj->m_targetGroupID,
                    .dualMode = obj->m_touchDualMode,
                    .holdMode = obj->m_touchHoldMode,
                    .toggleMode = obj->m_touchToggleMode,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1611: {
            return TriggerState {
                .props = {TriggerState::Count {
                    .itemID = obj->m_itemBlockAID,
                    .targetGroupID = obj->m_targetGroupID,
                    .targetCount = obj->m_targetCount,
                    .activateGroup = obj->m_activateGroup,
                    .multiActivate = obj->m_multiActivate,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1811: {
            return TriggerState {
                .props = {TriggerState::InstantCount {
                    .itemID = obj->m_itemBlockAID,
                    .targetGroupID = obj->m_targetGroupID,
                    .targetCount = obj->m_targetCount,
                    .activateGroup = obj->m_activateGroup,
                    .comparisonType = obj->m_comparisonType,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1817: {
            return TriggerState {
                .props = {TriggerState::Pickup {
                    .itemID = obj->m_itemBlockAID,
                    .count = obj->m_targetCount,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1815: {
            return TriggerState {
                .props = {TriggerState::Collision {
                    .blockAID = obj->m_itemBlockAID,
                    .blockBID = obj->m_blockBID,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        case 1812: {
            return TriggerState {
                .props = {TriggerState::OnDeath {
                    .targetGroupID = obj->m_targetGroupID,
                    .activateGroup = obj->m_activateGroup,
                }},
                .touchTrigger = obj->m_touchTriggered,
                .spawnTrigger = obj->m_spawnTriggered,
                .multiTrigger = obj->m_multiTrigger,
            };
        } break;

        default: {
            return std::nullopt;
        } break;
    }
}

void TriggerState::to(EffectGameObject* obj) const {
    std::visit(makeVisitor {
        [&](TriggerState::Color const& color) {
            obj->m_targetColorID = color.channel;
            obj->m_duration = color.fadeTime;
            obj->m_colColor = color.color;
            obj->m_opacity = color.opacity;
            obj->m_blending = color.blending;
            obj->m_playerColor1 = color.playerColor1;
            obj->m_playerColor2 = color.playerColor2;
            obj->m_copyColorID = color.copyID;
            obj->m_hsvValue = color.copyHSV;
            obj->m_copyOpacity = color.copyOpacity;
        },
        [&](TriggerState::Move const& move) {
            obj->m_move.x = move.x;
            obj->m_move.y = move.y;
            obj->m_lockToPlayerX = move.lockToPlayerX;
            obj->m_lockToPlayerY = move.lockToPlayerY;
            obj->m_easingType = move.easing;
            obj->m_easingRate = move.easingRate;
            obj->m_duration = move.time;
            obj->m_targetGroupID = move.targetGroupID;
            obj->m_useTarget = move.useTarget;
            obj->m_centerGroupID = move.followGroupID;
            obj->m_moveTargetType = move.followType;
        },
        [&](TriggerState::Stop const& stop) {
            obj->m_targetGroupID = stop.targetGroupID;
        },
        [&](TriggerState::Pulse const& pulse) {
            obj->m_targetGroupID = pulse.targetID;
            obj->m_pulseGroupMode = pulse.targetGroup;
            obj->m_pulseMainOnly = pulse.mainOnly;
            obj->m_pulseDetailOnly = pulse.detailOnly;
            obj->m_colColor = pulse.color;
            obj->m_pulseHSVMode = pulse.pulseHSV;
            obj->m_copyColorID = pulse.copyID;
            obj->m_hsvValue = pulse.copyHSV;
            obj->m_fadeInTime = pulse.fadeIn;
            obj->m_holdTime = pulse.hold;
            obj->m_fadeOutTime = pulse.fadeOut;
            obj->m_pulseExclusive = pulse.exclusive;
        },
        [&](TriggerState::Alpha const& alpha) {
            obj->m_targetGroupID = alpha.targetGroupID;
            obj->m_duration = alpha.time;
            obj->m_opacity = alpha.opacity;
        },
        [&](TriggerState::Toggle const& toggle) {
            obj->m_targetGroupID = toggle.targetGroupID;
            obj->m_activateGroup = toggle.activateGroup;
        },
        [&](TriggerState::Spawn const& spawn) {
            obj->m_targetGroupID = spawn.targetGroupID;
            obj->m_spawnDelay = spawn.delayTime;
            obj->m_editorDisabled = spawn.editorDisable;
        },
        [&](TriggerState::Rotate const& rotate) {
            obj->m_targetGroupID = rotate.targetGroupID;
            obj->m_centerGroupID = rotate.centerGroupID;
            obj->m_rotateDegrees = rotate.degrees;
            obj->m_times360 = rotate.times360;
            obj->m_lockObjectRotation = rotate.lockObjectRotation;
            obj->m_easingType = rotate.easing;
            obj->m_easingRate = rotate.easingRate;
            obj->m_duration = rotate.time;
        },
        [&](TriggerState::Follow const& follow) {
            obj->m_targetGroupID = follow.targetGroupID;
            obj->m_centerGroupID = follow.followGroupID;
            obj->m_followMod.x = follow.xMod;
            obj->m_followMod.y = follow.yMod;
            obj->m_duration = follow.time;
        },
        [&](TriggerState::Shake const& shake) {
            obj->m_shakeStrength = shake.strength;
            obj->m_shakeInterval = shake.interval;
            obj->m_duration = shake.duration;
        },
        [&](TriggerState::Animation const& animation) {
            obj->m_targetGroupID = animation.targetGroupID;
            obj->m_animationID = animation.animationID;
        },
        [&](TriggerState::FollowPlayerY const& followPlayerY) {
            obj->m_targetGroupID = followPlayerY.targetGroupID;
            obj->m_followYSpeed = followPlayerY.speed;
            obj->m_followYDelay = followPlayerY.delay;
            obj->m_followYOffset = followPlayerY.offset;
            obj->m_followYMaxSpeed = followPlayerY.maxSpeed;
            obj->m_duration = followPlayerY.time;
        },
        [&](TriggerState::Touch const& touch) {
            obj->m_targetGroupID = touch.targetGroupID;
            obj->m_touchDualMode = touch.dualMode;
            obj->m_touchHoldMode = touch.holdMode;
            obj->m_touchToggleMode = touch.toggleMode;
        },
        [&](TriggerState::Count const& count) {
            obj->m_itemBlockAID = count.itemID;
            obj->m_targetGroupID = count.targetGroupID;
            obj->m_targetCount = count.targetCount;
            obj->m_activateGroup = count.activateGroup;
            obj->m_multiActivate = count.multiActivate;
        },
        [&](TriggerState::InstantCount const& instantCount) {
            obj->m_itemBlockAID = instantCount.itemID;
            obj->m_targetGroupID = instantCount.targetGroupID;
            obj->m_targetCount = instantCount.targetCount;
            obj->m_activateGroup = instantCount.activateGroup;
            obj->m_comparisonType = instantCount.comparisonType;
        },
        [&](TriggerState::Pickup const& pickup) {
            obj->m_itemBlockAID = pickup.itemID;
            obj->m_targetCount = pickup.count;
        },
        [&](TriggerState::Collision const& collision) {
            obj->m_itemBlockAID = collision.blockAID;
            obj->m_blockBID = collision.blockBID;
        },
        [&](TriggerState::OnDeath const& onDeath) {
            obj->m_targetGroupID = onDeath.targetGroupID;
            obj->m_activateGroup = onDeath.activateGroup;
        },
    }, props);

    obj->m_touchTriggered = touchTrigger;
    obj->m_spawnTriggered = spawnTrigger;
    obj->m_multiTrigger = multiTrigger;

    obj->updateLabel();
    LevelEditorLayer::get()->resetEffectTriggerOptim(obj, nullptr);
}

std::unique_ptr<EditorEvent> EditorEvent::unique() const {
    return std::unique_ptr<EditorEvent>(static_cast<EditorEvent*>(this->clone()));
}

ObjEventData::ObjEventData(Ref<GameObject> obj)
  : obj(obj) {}

// ObjPlaced

std::string ObjPlaced::toDiffString() const {
    return fmtDiffString("add", obj->m_objectID, pos);
}

EditorEventData* ObjPlaced::clone() const {
    return new ObjPlaced(obj, pos);
}

void ObjPlaced::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
    obj->deactivateObject(true);
}

void ObjPlaced::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
    EditorUI::get()->moveObject(obj, pos - obj->getPosition());
    EditorUI::get()->selectObject(obj, true);
}

std::vector<Detail> ObjPlaced::details() const {
    return {{ .info = fmt::format("ID {}", obj->m_objectID) }};
}

// ObjRemoved

std::string ObjRemoved::toDiffString() const {
    return fmtDiffString("rem", obj);
}

EditorEventData* ObjRemoved::clone() const {
    return new ObjRemoved(obj);
}

void ObjRemoved::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
    EditorUI::get()->selectObject(obj, true);
}

void ObjRemoved::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
    obj->deactivateObject(true);
}

std::vector<Detail> ObjRemoved::details() const {
    return {};
}

// ObjPasted

std::string ObjPasted::toDiffString() const {
    return fmtDiffString("pst", obj, src);
}

EditorEventData* ObjPasted::clone() const {
    return new ObjPasted(obj, src);
}

void ObjPasted::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
    obj->deactivateObject(true);
}

void ObjPasted::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
    EditorUI::get()->selectObject(obj, true);
}

std::vector<Detail> ObjPasted::details() const {
    return {};
}

// ObjMoved

std::string ObjMoved::toDiffString() const {
    return fmtDiffString("mov", obj, pos);
}

EditorEventData* ObjMoved::clone() const {
    return new ObjMoved(obj, pos);
}

void ObjMoved::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjMoved::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

std::vector<Detail> ObjMoved::details() const {
    std::vector<Detail> res;
    auto dx = pos.to.x - pos.from.x;
    auto dy = pos.to.y - pos.from.y;
    if (dx != 0.f) {
        res.push_back({
            .info = numToString(std::fabsf(dx)),
            .icon = dx > 0.f ? "edit_rightBtn2_001.png" : "edit_leftBtn2_001.png"
        });
    }
    if (dy != 0.f) {
        res.push_back({
            .info = numToString(std::fabsf(dy)),
            .icon = dy > 0.f ? "edit_upBtn2_001.png" : "edit_downBtn2_001.png"
        });
    }
    return res;
}

// ObjRotated

std::string ObjRotated::toDiffString() const {
    return fmtDiffString("rot", obj, pos, angle);
}

EditorEventData* ObjRotated::clone() const {
    return new ObjRotated(obj, pos, angle);
}

void ObjRotated::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->rotateObjects(
        CCArray::createWithObject(obj), angle.from - obj->getRotation(), obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjRotated::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->rotateObjects(
        CCArray::createWithObject(obj), angle.to - obj->getRotation(), obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

std::vector<Detail> ObjRotated::details() const {
    auto delta = angle.to - angle.from;
    return {{
        .info = fmt::format("{}°", std::fabsf(delta)),
        .icon = delta > 0.f ? "edit_cwBtn_001.png" : "edit_ccwBtn_001.png"
    }};
}

// ObjScaled

std::string ObjScaled::toDiffString() const {
    return fmtDiffString("scl", obj, pos, scale);
}

EditorEventData* ObjScaled::clone() const {
    return new ObjScaled(obj, pos, scale);
}

void ObjScaled::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->scaleObjects(
        CCArray::createWithObject(obj), scale.from, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjScaled::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->scaleObjects(
        CCArray::createWithObject(obj), scale.to, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

std::vector<Detail> ObjScaled::details() const {
    return {{
        .info = fmt::format("{}x", scale.to),
        .icon = "scale.png"_spr
    }};
}

// ObjFlipX

std::string ObjFlipX::toDiffString() const {
    return fmtDiffString("fpx", obj, pos, flip);
}

EditorEventData* ObjFlipX::clone() const {
    return new ObjFlipX(obj, pos, flip);
}

void ObjFlipX::undo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsX(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjFlipX::redo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsX(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

std::vector<Detail> ObjFlipX::details() const {
    return {};
}

// ObjFlipY

std::string ObjFlipY::toDiffString() const {
    return fmtDiffString("fpy", obj, pos, flip);
}

EditorEventData* ObjFlipY::clone() const {
    return new ObjFlipY(obj, pos, flip);
}

void ObjFlipY::undo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsY(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjFlipY::redo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsY(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

std::vector<Detail> ObjFlipY::details() const {
    return {};
}

// ObjColored

std::string ObjColored::toDiffString() const {
    return fmtDiffString("col", obj, color);
}

EditorEventData* ObjColored::clone() const {
    return new ObjColored(obj, color);
}

void ObjColored::undo() const {
    color.from.to(obj);
}

void ObjColored::redo() const {
    color.to.to(obj);
}

std::vector<Detail> ObjColored::details() const {
    return {};
}

// ObjPropsChanged

std::string ObjPropsChanged::toDiffString() const {
    return fmtDiffString("prp", obj, state);
}

EditorEventData* ObjPropsChanged::clone() const {
    return new ObjPropsChanged(obj, state);
}

void ObjPropsChanged::undo() const {
    auto _ = BlockAll();
    state.from.to(obj);
}

void ObjPropsChanged::redo() const {
    auto _ = BlockAll();
    state.to.to(obj);
}

std::vector<Detail> ObjPropsChanged::details() const {
    std::vector<Detail> res;
    if (state.from.groups != state.to.groups) {
        std::string info = "Groups ";
        bool first = true;
        for (auto& grp : state.to.groups) {
            if (!first) {
                info += ", ";
            }
            first = false;
            info += std::to_string(grp);
        }
        res.push_back({ .info = info });
    }
    if (state.from.editorLayer1 != state.to.editorLayer1) {
        res.push_back({
            .info = fmt::format("Layer 1 to {}", state.to.editorLayer1)
        });
    }
    if (state.from.editorLayer2 != state.to.editorLayer2) {
        res.push_back({
            .info = fmt::format("Layer 2 to {}", state.to.editorLayer2)
        });
    }
    if (state.from.zOrder != state.to.zOrder) {
        res.push_back({
            .info = fmt::format("Z Order to {}", state.to.zOrder)
        });
    }
    if (state.from.zLayer != state.to.zLayer) {
        res.push_back({
            .info = fmt::format("Z Layer to {}", zLayerToString(state.to.zLayer))
        });
    }
    if (state.to.dontEnter) {
        res.push_back({ .info = "Don't Enter" });
    }
    if (state.to.dontFade) {
        res.push_back({ .info = "Don't Fade" });
    }
    if (state.to.groupParent) {
        res.push_back({ .info = "Group Parent" });
    }
    if (state.to.highDetail) {
        res.push_back({ .info = "High Detail" });
    }
    return res;
}

// ObjSelected

std::string ObjSelected::toDiffString() const {
    return fmtDiffString("sel", obj);
}

EditorEventData* ObjSelected::clone() const {
    return new ObjSelected(obj);
}

void ObjSelected::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
}

void ObjSelected::redo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

std::vector<Detail> ObjSelected::details() const {
    return {};
}

// ObjDeselected

std::string ObjDeselected::toDiffString() const {
    return fmtDiffString("dsl", obj);
}

EditorEventData* ObjDeselected::clone() const {
    return new ObjDeselected(obj);
}

void ObjDeselected::undo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

void ObjDeselected::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
}

std::vector<Detail> ObjDeselected::details() const {
    return {};
}

// StartPosChanged

std::string StartPosChanged::toDiffString() const {
    return fmtDiffString("str", obj, state);
}

EditorEventData* StartPosChanged::clone() const {
    return new StartPosChanged(static_cast<StartPosObject*>(obj.data()), state);
}

void StartPosChanged::undo() const {
    state.from.to(static_cast<StartPosObject*>(obj.data())->m_levelSettings);
}

void StartPosChanged::redo() const {
    state.to.to(static_cast<StartPosObject*>(obj.data())->m_levelSettings);
}

std::vector<Detail> StartPosChanged::details() const {
    // todo
    return {};
}

// TriggerPropsChanged

std::string TriggerPropsChanged::toDiffString() const {
    return fmtDiffString("trg", obj, state);
}

EditorEventData* TriggerPropsChanged::clone() const {
    return new TriggerPropsChanged(static_cast<EffectGameObject*>(obj.data()), state);
}

void TriggerPropsChanged::undo() const {
    state.from.to(static_cast<EffectGameObject*>(obj.data()));
}

void TriggerPropsChanged::redo() const {
    state.to.to(static_cast<EffectGameObject*>(obj.data()));
}

std::vector<Detail> TriggerPropsChanged::details() const {
    // todo
    return {};
}

// ColorChannelEvent

std::string ColorChannelEvent::toDiffString() const {
    return fmtDiffString("chn", channel, state);
}

EditorEventData* ColorChannelEvent::clone() const {
    return new ColorChannelEvent(channel, state);
}

void ColorChannelEvent::undo() const {
    if (auto action = GJEffectManager::fromLevelSetting()->getColorAction(channel)) {
        state.from.to(action);
    }
}

void ColorChannelEvent::redo() const {
    if (auto action = GJEffectManager::fromLevelSetting()->getColorAction(channel)) {
        state.to.to(action);
    }
}

std::vector<Detail> ColorChannelEvent::details() const {
    return {};
}

CCNode* ColorChannelEvent::icon() const {
    return CCSprite::createWithSpriteFrameName("color.png"_spr);
}

std::string ColorChannelEvent::desc() const {
    return fmt::format("Channel {} Changed", channel);
}

// LevelSettingsChanged

std::string LevelSettingsChanged::toDiffString() const {
    return fmtDiffString("set", state);
}

EditorEventData* LevelSettingsChanged::clone() const {
    return new LevelSettingsChanged(state);
}

void LevelSettingsChanged::undo() const {
    state.from.to(LevelEditorLayer::get()->m_levelSettings);
}

void LevelSettingsChanged::redo() const {
    state.to.to(LevelEditorLayer::get()->m_levelSettings);
}

std::vector<Detail> LevelSettingsChanged::details() const {
    return {};
}

CCNode* LevelSettingsChanged::icon() const {
    return CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
}

std::string LevelSettingsChanged::desc() const {
    return "Level Settings Changed";
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}
