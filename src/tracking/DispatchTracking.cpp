
#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/ColorAction.hpp>

#define BLOCKED_CALL(...) \
    {\
        auto block = BlockAll();\
        __VA_ARGS__;\
    }

static CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets) {
    if (target) {
        return CCArrayExt<GameObject>(CCArray::createWithObject(target));
    }
    else {
        return targets;
    }
}

template <class T>
struct Bubble {
    std::vector<T> events;
    static inline Bubble<T>* s_current = nullptr;
    Bubble() {
        if (!s_current) {
            s_current = this;
        }
    }
    ~Bubble() {
        if (s_current == this) {
            s_current = nullptr;
            if (events.size()) {
                post(MultiObjEvent(std::move(events)));
            }
        }
    }
    void cancel() {
        if (s_current == this) {
            s_current = nullptr;
        }
    }
    template <class... Args>
    static void push(Args&&... args) {
        if (BlockAll::blocked()) return;
        if (s_current) {
            s_current->events.emplace_back(std::forward<Args>(args)...);
        }
        else {
            post(MultiObjEvent<T>({ T(std::forward<Args>(args)...) }));
        }
    }
private:
    template <class T>
    static void post(T&& t) {
        if (EditorUI::get() && !BlockAll::blocked()) {
            t.post();
        }
    }
};

class $modify(LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        BLOCKED_CALL(LevelEditorLayer::addSpecial(obj));
        Bubble<ObjPlaced>::push(obj, obj->getPosition());
    }

    CCArray* createObjectsFromString(gd::string str, bool undo) {
        auto bubble = Bubble<ObjPlaced>();
        return LevelEditorLayer::createObjectsFromString(str, undo);
    }

    void createObjectsFromSetup(gd::string str) {
        auto bubble = Bubble<ObjPlaced>();
        LevelEditorLayer::createObjectsFromSetup(str);
        bubble.cancel();
    }

    void removeSpecial(GameObject* obj) {
        BLOCKED_CALL(LevelEditorLayer::removeSpecial(obj));
        Bubble<ObjRemoved>::push(obj);
    }

    void pasteAtributeState(GameObject* target, CCArray* targets) {
        std::vector<ObjState> states;
        for (auto& obj : iterTargets(target, targets)) {
            states.push_back(ObjState::from(target));
        }
        LevelEditorLayer::pasteAtributeState(target, targets);
        size_t i = 0;
        auto bubble = Bubble<ObjPropsChanged>();
        for (auto& obj : iterTargets(target, targets)) {
            Bubble<ObjPropsChanged>::push(obj, Transform { states.at(i++), ObjState::from(obj) });
        }
    }

    void pasteColorState(GameObject* target, CCArray* targets) {
        std::vector<std::pair<int, ccHSVValue>> oldBases;
        std::vector<std::pair<int, ccHSVValue>> oldDetails;
        for (auto& obj : iterTargets(target, targets)) {
            if (target->m_baseColor) {
                oldBases.emplace_back(
                    target->m_baseColor->m_colorID,
                    target->m_baseColor->m_hsv
                );
            }
            else {
                oldBases.emplace_back();
            }
            if (target->m_detailColor) {
                oldDetails.emplace_back(
                    target->m_detailColor->m_colorID,
                    target->m_detailColor->m_hsv
                );
            }
            else {
                oldDetails.emplace_back();
            }
        }
        LevelEditorLayer::pasteColorState(target, targets);
        auto bubble = Bubble<ObjColorPasted>();
        size_t i = 0;
        for (auto& obj : iterTargets(target, targets)) {
            Bubble<ObjColorPasted>::push(
                obj,
                Transform {
                    oldBases.at(i).first,
                    (obj->m_baseColor ? obj->m_baseColor->m_colorID : 0)
                },
                Transform {
                    oldBases.at(i).second,
                    (obj->m_baseColor ? obj->m_baseColor->m_hsv : ccHSVValue())
                },
                Transform {
                    oldDetails.at(i).first,
                    (obj->m_detailColor ? obj->m_detailColor->m_colorID : 0)
                },
                Transform {
                    oldDetails.at(i).second,
                    (obj->m_detailColor ? obj->m_detailColor->m_hsv : ccHSVValue())
                }
            );
            i += 1;
        }
    }
};

class $modify(EditorUI) {
    std::vector<std::pair<CCPoint, float>> originalScales;

    void moveObject(GameObject* obj, CCPoint to) {
        auto from = obj->getPosition();
        BLOCKED_CALL(EditorUI::moveObject(obj, to));
        Bubble<ObjMoved>::push(obj, Transform<CCPoint> { from, from + to });
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjMoved>();
        EditorUI::moveObjectCall(command);
    }

    void transformObjectCall(EditCommand command) {
        auto bubble0 = Bubble<ObjFlipX>();
        auto bubble1 = Bubble<ObjFlipY>();
        auto bubble2 = Bubble<ObjRotated>();
        auto bubble3 = Bubble<ObjScaled>();
        EditorUI::transformObjectCall(command);
    }

    void transformObject(GameObject* obj, EditCommand command, bool snap) {
        switch (command) {
            case EditCommand::RotateCCW:  case EditCommand::RotateCCW45:
            case EditCommand::RotateCW:   case EditCommand::RotateCW45:
            case EditCommand::RotateSnap: case EditCommand::RotateFree: {
                auto prevAngle = obj->getRotation();
                auto prevPos = obj->getPosition();
                BLOCKED_CALL(EditorUI::transformObject(obj, command, snap));
                Bubble<ObjRotated>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform { prevAngle, obj->getRotation() }
                );
            } break;

            case EditCommand::FlipX: {
                auto prevPos = obj->getPosition();
                BLOCKED_CALL(EditorUI::transformObject(obj, command, snap));
                Bubble<ObjFlipX>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform { false, true }
                );
            } break;

            case EditCommand::FlipY: {
                auto prevPos = obj->getPosition();
                BLOCKED_CALL(EditorUI::transformObject(obj, command, snap));
                Bubble<ObjFlipY>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform { false, true }
                );
            } break;

            default: {
                EditorUI::transformObject(obj, command, snap);
            } break;
        }
    }

    void deselectAll() {
        auto bubble = Bubble<ObjDeselected>();
        EditorUI::deselectAll();
    }

    void selectObjects(CCArray* objs, bool filter) {
        auto bubble = Bubble<ObjSelected>();
        EditorUI::selectObjects(objs, filter);
    }

    void scaleChangeBegin() {
        m_fields->originalScales = {};
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            m_fields->originalScales.emplace_back(obj->getPosition(), obj->getScale());
        }
        BLOCKED_CALL(EditorUI::scaleChangeBegin());
    }

    void scaleChanged(float scale) {
        BLOCKED_CALL(EditorUI::scaleChanged(scale));
    }

    void scaleChangeEnded() {
        BLOCKED_CALL(EditorUI::scaleChangeEnded());
        size_t i = 0;
        auto bubble = Bubble<ObjScaled>();
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            Bubble<ObjScaled>::push(
                obj,
                Transform { m_fields->originalScales.at(i).first, obj->getPosition() },
                Transform { m_fields->originalScales.at(i).second, obj->getScale() }
            );
            i += 1;
        }
    }

    void scaleObjects(CCArray* objs, float scale, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> scales;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            scales.push_back({ obj, obj->getPosition(), obj->m_scale });
        }
        BLOCKED_CALL(EditorUI::scaleObjects(objs, scale, center));
        auto bubble = Bubble<ObjScaled>();
        for (auto& [obj, pos, from] : scales) {
            Bubble<ObjScaled>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<float> { from, obj->m_scale }
            );
        }
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> angles;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            angles.push_back({ obj, obj->getPosition(), obj->m_rotation });
        }
        BLOCKED_CALL(EditorUI::rotateObjects(objs, angle, center));
        auto bubble = Bubble<ObjRotated>();
        for (auto& [obj, pos, from] : angles) {
            Bubble<ObjRotated>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<float> { from, obj->m_rotation }
            );
        }
    }

    void flipObjectsX(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), false });
        }
        BLOCKED_CALL(EditorUI::flipObjectsX(objs));
        auto bubble = Bubble<ObjFlipX>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipX>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, true }
            );
        }
    }

    void flipObjectsY(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), false });
        }
        BLOCKED_CALL(EditorUI::flipObjectsY(objs));
        auto bubble = Bubble<ObjFlipY>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipY>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, true }
            );
        }
    }
};

class $modify(GameObject) {
    void selectObject(ccColor3B color) {
        GameObject::selectObject(color);
        Bubble<ObjSelected>::push(this);
    }

    void deselectObject() {
        GameObject::deselectObject();
        Bubble<ObjDeselected>::push(this);
    }

    void removeFromGroup(int group) {
        auto before = ObjState::from(this);
        GameObject::removeFromGroup(group);
        auto now = ObjState::from(this);
        Bubble<ObjPropsChanged>::push(this, Transform { before, now });
    }

    void addToGroup(int group) {
        auto before = ObjState::from(this);
        GameObject::addToGroup(group);
        auto now = ObjState::from(this);
        Bubble<ObjPropsChanged>::push(this, Transform { before, now });
    }
};

class $modify(CustomizeObjectLayer) {
    std::vector<ccHSVValue> hsv;

    GJSpriteColor* colorFor(GameObject* obj) {
        if (m_selectedMode == 1) {
            return obj->m_baseColor;
        }
        else if (m_selectedMode == 2) {
            return obj->m_detailColor;
        }
        return nullptr;
    }

    void updateSelected(int channelID) {
        std::vector<int> oldTargetIDs;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            oldTargetIDs.push_back(colorFor(obj)->m_colorID);
        }
        CustomizeObjectLayer::updateSelected(channelID);
        auto bubble = Bubble<ObjColored>();
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            Bubble<ObjColored>::push(
                obj, m_selectedMode == 2,
                Transform<int> { oldTargetIDs.at(i++), colorFor(obj)->m_colorID }
            );
        }
    }

    void onHSV(CCObject* sender) {
        m_fields->hsv.clear();
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            m_fields->hsv.push_back(colorFor(obj)->m_hsv);
        }
        CustomizeObjectLayer::onHSV(sender);
    }

    void hsvPopupClosed(HSVWidgetPopup* popup, ccHSVValue value) {
        CustomizeObjectLayer::hsvPopupClosed(popup, value);
        auto bubble = Bubble<ObjHSVChanged>();
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            Bubble<ObjHSVChanged>::push(
                obj, m_selectedMode == 2,
                Transform { m_fields->hsv.at(i++), colorFor(obj)->m_hsv }
            );
        }
    }
};

struct $modify(ColorSelectPopup) {
    ColorState state;

    bool init(EffectGameObject* target, CCArray* targets, ColorAction* action) {
        if (!ColorSelectPopup::init(target, targets, action))
            return false;

        if (action) {
            m_fields->state = ColorState::from(action);
        }
        
        return true;
    }

    void closeColorSelect(CCObject* sender) {
        ColorSelectPopup::closeColorSelect(sender);
        if (m_colorAction) {
            auto state = ColorState::from(m_colorAction);
            if (m_fields->state != state) {
                ColorChannelEvent(
                    m_colorAction->m_colorID,
                    Transform { m_fields->state, state }
                ).post();
            }
        }
    }
};

class $modify(SetGroupIDLayer) {
    std::vector<ObjState> states;

    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        if (obj) {
            m_fields->states = { ObjState::from(obj) }; 
        }
        else for (auto& obj : CCArrayExt<GameObject>(objs)) {
            m_fields->states.push_back(ObjState::from(obj));
        }

        return true;
    }

    void onAddGroup(CCObject* sender) {
        BLOCKED_CALL(SetGroupIDLayer::onAddGroup(sender));
    }

    void onClose(CCObject* sender) {
        SetGroupIDLayer::onClose(sender);
        auto bubble = Bubble<ObjPropsChanged>();
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            auto state = ObjState::from(obj);
            if (m_fields->states.at(i) != state) {
                Bubble<ObjPropsChanged>::push(
                    obj, Transform { m_fields->states.at(i), state }
                );
            }
            i += 1;
        }
    }
};
