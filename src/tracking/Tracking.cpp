#include "Tracking.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/CustomizeObjectLayer.hpp>
#include <Geode/binding/HSVWidgetPopup.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/utils/cocos.hpp>
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
    return fmt::format(
        "{},{},{},{},{},{}",
        toDiffString(value.color), value.opacity, value.blending,
        value.playerColor, value.copyColorID, toDiffString(value.copyHSV)
    );
}

std::string toDiffString(ObjColorState const& value) {
    return fmt::format(
        "{},{},{},{},{}",
        value.base, toDiffString(value.baseHSV),
        value.detail, toDiffString(value.detailHSV),
        value.glow
    );
}

std::string toDiffString(ObjState const& value) {
    return fmt::format(
        "{},{},{},{},{},{},{},{},{}",
        toDiffString(value.groups), value.editorLayer1, value.editorLayer2, value.zOrder, 
        toDiffString(value.zLayer), value.dontFade, value.dontEnter, value.groupParent,
        value.highDetail
    );
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

bool ColorState::operator==(ColorState const& other) const {
    return color == other.color &&
        opacity == other.opacity &&
        blending == other.blending &&
        playerColor == other.playerColor &&
        copyColorID == other.copyColorID &&
        copyHSV == other.copyHSV;
}

bool ColorState::operator!=(ColorState const& other) const {
    return !(*this == other);
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

bool ObjColorState::operator==(ObjColorState const& other) const {
    return base == other.base &&
        baseHSV == other.baseHSV &&
        detail == other.detail &&
        detailHSV == other.detailHSV &&
        glow == other.glow;
}

bool ObjColorState::operator!=(ObjColorState const& other) const {
    return !(*this == other);
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

bool ObjState::operator==(ObjState const& other) const {
    return groups == other.groups &&
        editorLayer1 == other.editorLayer1 &&
        editorLayer2 == other.editorLayer2 &&
        zOrder == other.zOrder &&
        zLayer == other.zLayer &&
        dontFade == other.dontFade && 
        dontEnter == other.dontEnter && 
        groupParent == other.groupParent && 
        highDetail == other.highDetail;
}

bool ObjState::operator!=(ObjState const& other) const {
    return !(*this == other);
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

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}
