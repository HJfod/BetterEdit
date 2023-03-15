
#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/ColorAction.hpp>

#define BLOCKED_CALL(...) \
    {\
        auto block = BlockAll();\
        __VA_ARGS__;\
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
};

class $modify(EditorUI) {
    void moveObject(GameObject* obj, CCPoint to) {
        auto from = obj->getPosition();
        BLOCKED_CALL(EditorUI::moveObject(obj, to));
        Bubble<ObjMoved>::push(obj, Transform<CCPoint> { from, from + to });
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjMoved>();
        EditorUI::moveObjectCall(command);
    }

    void deselectAll() {
        auto bubble = Bubble<ObjDeselected>();
        EditorUI::deselectAll();
    }

    void selectObjects(CCArray* objs, bool filter) {
        auto bubble = Bubble<ObjSelected>();
        EditorUI::selectObjects(objs, filter);
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
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedX });
        }
        BLOCKED_CALL(EditorUI::flipObjectsX(objs));
        auto bubble = Bubble<ObjFlipX>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipX>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, obj->m_isFlippedX }
            );
        }
    }

    void flipObjectsY(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedY });
        }
        BLOCKED_CALL(EditorUI::flipObjectsY(objs));
        auto bubble = Bubble<ObjFlipY>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipY>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, obj->m_isFlippedY }
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
        if (m_targetObject) {
            oldTargetIDs.push_back(colorFor(m_targetObject)->m_colorID);
        }
        else for (auto node : CCArrayExt<GameObject>(m_targetObjects)) {
            oldTargetIDs.push_back(colorFor(node)->m_colorID);
        }
        CustomizeObjectLayer::updateSelected(channelID);
        if (m_targetObject) {
            Bubble<ObjColored>::push(
                m_targetObject, m_selectedMode == 2,
                Transform<int> { oldTargetIDs.at(0), colorFor(m_targetObject)->m_colorID }
            );
        }
        else {
            auto bubble = Bubble<ObjColored>();
            size_t i = 0;
            for (auto node : CCArrayExt<GameObject>(m_targetObjects)) {
                Bubble<ObjColored>::push(
                    node, m_selectedMode == 2,
                    Transform<int> { oldTargetIDs.at(i), colorFor(node)->m_colorID }
                );
                i += 1;
            }
        }
    }

    void onHSV(CCObject* sender) {
        if (m_targetObject) {
            m_fields->hsv = { colorFor(m_targetObject)->m_hsv };
        }
        else {
            m_fields->hsv.clear();
            for (auto node : CCArrayExt<GameObject>(m_targetObjects)) {
                m_fields->hsv.push_back(colorFor(node)->m_hsv);
            }
        }
        CustomizeObjectLayer::onHSV(sender);
    }

    void hsvPopupClosed(HSVWidgetPopup* popup, ccHSVValue value) {
        CustomizeObjectLayer::hsvPopupClosed(popup, value);
        if (m_targetObject) {
            Bubble<ObjHSVChanged>::push(
                m_targetObject, m_selectedMode == 2,
                Transform { m_fields->hsv.front(), colorFor(m_targetObject)->m_hsv }
            );
        }
        else {
            auto bubble = Bubble<ObjHSVChanged>();
            size_t i = 0;
            for (auto node : CCArrayExt<GameObject>(m_targetObjects)) {
                Bubble<ObjHSVChanged>::push(
                    m_targetObject, m_selectedMode == 2,
                    Transform { m_fields->hsv.at(i), colorFor(m_targetObject)->m_hsv }
                );
                i += 1;
            }
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
            log::info("m_colorAction: {}", m_colorAction);
            if (m_fields->state != state) {
                ColorChannelEvent(
                    m_colorAction->m_colorID,
                    Transform { m_fields->state, state }
                ).post();
            }
        }
    }
};
