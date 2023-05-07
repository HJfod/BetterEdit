#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/CCLayerColor.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include <other/Utils.hpp>

static int TRACKING_HOOK_PRIORITY = 999;

#define BLOCKED_CALL(...) \
    {\
        auto block = BlockAll();\
        __VA_ARGS__;\
    }

#define BLOCKED_CALL_TO(var_, ...) \
    {\
        auto block = BlockAll();\
        var_ = __VA_ARGS__;\
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
    template <class E>
    static void post(E&& t) {
        if (EditorUI::get() && !BlockAll::blocked()) {
            t.post();
        }
    }
};

class $modify(EditorPauseLayer) {
    void saveLevel() {
        BLOCKED_CALL(EditorPauseLayer::saveLevel());
    }

    void onSaveAndPlay(CCObject* sender) {
        BLOCKED_CALL(EditorPauseLayer::onSaveAndPlay(sender));
    }

    void onExitEditor(CCObject* sender) {
        BLOCKED_CALL(EditorPauseLayer::onExitEditor(sender));
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level) {
        bool ret;
        BLOCKED_CALL_TO(ret, PlayLayer::init(level));
        return ret;
    }
};

class $modify(LevelEditorLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelEditorLayer::addSpecial", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelEditorLayer::createObjectsFromString", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelEditorLayer::createObjectsFromSetup", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelEditorLayer::removeSpecial", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelEditorLayer::pasteAtributeState", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelEditorLayer::pasteColorState", TRACKING_HOOK_PRIORITY);
    }

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
            states.push_back(ObjState::from(obj));
        }
        LevelEditorLayer::pasteAtributeState(target, targets);
        size_t i = 0;
        auto bubble = Bubble<ObjPropsChanged>();
        for (auto& obj : iterTargets(target, targets)) {
            Bubble<ObjPropsChanged>::push(obj, Transform { states.at(i++), ObjState::from(obj) });
        }
    }

    void pasteColorState(GameObject* target, CCArray* targets) {
        std::vector<ObjColorState> olds;
        for (auto& obj : iterTargets(target, targets)) {
            olds.push_back(ObjColorState::from(obj));
        }
        LevelEditorLayer::pasteColorState(target, targets);
        auto bubble = Bubble<ObjColored>();
        size_t i = 0;
        for (auto& obj : iterTargets(target, targets)) {
            Bubble<ObjColored>::push(
                obj, Transform { olds.at(i++), ObjColorState::from(obj) }
            );
        }
    }
};

struct FreeMoveData {
    CCPoint pos;
    float angle;
    float scale;
    bool flipX;
    bool flipY;
};

class $modify(EditorUI) {
    std::vector<std::pair<CCPoint, float>> originalScales;
    std::vector<FreeMoveData> freeMoveStart;
    bool freeMoveFlippedX;
    bool freeMoveFlippedY;
    std::unique_ptr<Bubble<ObjTransformed>> freeMoveBubble;

    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::onDuplicate", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::onCreate", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::onDeleteSelected", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::deleteObject", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::moveObject", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::moveObjectCall", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::transformObjectCall", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::transformObject", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::deselectAll", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::selectObjects", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::ccTouchBegan", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::ccTouchMoved", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::ccTouchEnded", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::scaleChangeBegin", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::scaleChanged", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::scaleChangeEnded", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::scaleObjects", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::rotateObjects", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::flipObjectsX", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::flipObjectsY", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::alignObjects", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("EditorUI::onPlaytest", TRACKING_HOOK_PRIORITY);
    }

    void onDuplicate(CCObject* sender) {
        std::vector<GameObject*> src;
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            src.push_back(obj);
        }
        BLOCKED_CALL(EditorUI::onDuplicate(sender));
        auto bubble = Bubble<ObjPasted>();
        size_t i = 0;
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            Bubble<ObjPasted>::push(obj, src.at(i++));
        }
    }

    bool onCreate() {
        auto b1 = Bubble<ObjSelected>();
        auto b2 = Bubble<ObjDeselected>();
        auto b = EditorUI::onCreate();
        b1.cancel();
        b2.cancel();
        return b;
    }

    void onDelete(CCObject* sender) {
        auto b2 = Bubble<ObjDeselected>();
        EditorUI::onDelete(sender);
        b2.cancel();
    }

    void onDeleteSelected(CCObject* sender) {
        auto bubble1 = Bubble<ObjRemoved>();
        auto bubble2 = Bubble<ObjDeselected>();
        EditorUI::onDeleteSelected(sender);
        bubble2.cancel();
    }

    void deleteObject(GameObject* obj, bool filter) {
        BLOCKED_CALL(EditorUI::deleteObject(obj, filter));
        Bubble<ObjRemoved>::push(obj);
    }

    void moveObject(GameObject* obj, CCPoint to) {
        auto from = obj->getPosition();
        BLOCKED_CALL(EditorUI::moveObject(obj, to));
        Bubble<ObjTransformed>::push(
            obj,
            Transform { from, from + to },
            Transform<float>::zero(),
            Transform<float>::zero(),
            Transform<bool>::zero(),
            Transform<bool>::zero()
        );
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjTransformed>();
        EditorUI::moveObjectCall(command);
    }

    void transformObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjTransformed>();
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
                Bubble<ObjTransformed>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform { prevAngle, obj->getRotation() },
                    Transform<float>::zero(),
                    Transform<bool>::zero(),
                    Transform<bool>::zero()
                );
            } break;

            case EditCommand::FlipX: {
                auto prevPos = obj->getPosition();
                BLOCKED_CALL(EditorUI::transformObject(obj, command, snap));
                Bubble<ObjTransformed>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform<float>::zero(),
                    Transform<float>::zero(),
                    Transform { false, true },
                    Transform<bool>::zero()
                );
                m_fields->freeMoveFlippedX ^= 1;
            } break;

            case EditCommand::FlipY: {
                auto prevPos = obj->getPosition();
                BLOCKED_CALL(EditorUI::transformObject(obj, command, snap));
                Bubble<ObjTransformed>::push(
                    obj,
                    Transform { prevPos, obj->getPosition() },
                    Transform<float>::zero(),
                    Transform<float>::zero(),
                    Transform<bool>::zero(),
                    Transform { false, true }
                );
                m_fields->freeMoveFlippedY ^= 1;
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
 
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (EditorUI::ccTouchBegan(touch, event)) {
            m_fields->freeMoveStart = {};
            m_fields->freeMoveFlippedX = false;
            m_fields->freeMoveFlippedY = false;
            for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
                m_fields->freeMoveStart.push_back({
                    .pos = obj->getPosition(),
                    .angle = obj->getRotation(),
                    .scale = obj->getScale(),
                    .flipX = false,
                    .flipY = false,
                });
            }
            return true;
        }
        return false;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        {
            auto bubble = Bubble<ObjTransformed>();
            EditorUI::ccTouchMoved(touch, event);
            bubble.cancel();
        }
        if (m_freeMovingObject && !m_fields->freeMoveBubble.get()) {
            m_fields->freeMoveBubble = std::make_unique<Bubble<ObjTransformed>>();
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (m_freeMovingObject) {
            BLOCKED_CALL(EditorUI::ccTouchEnded(touch, event));
            if (m_fields->freeMoveBubble) {
                m_fields->freeMoveBubble->cancel();
                m_fields->freeMoveBubble = nullptr;
            }
            auto bubble = Bubble<ObjTransformed>();
            size_t i = 0;
            for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
                auto& start = m_fields->freeMoveStart.at(i++);
                if (start.pos != obj->getPosition()) {
                    Bubble<ObjTransformed>::push(
                        obj,
                        Transform { start.pos, obj->getPosition() },
                        Transform { start.angle, obj->getRotation() },
                        Transform { start.scale, obj->getScale() },
                        Transform { start.flipX, m_fields->freeMoveFlippedX },
                        Transform { start.flipY, m_fields->freeMoveFlippedY }
                    );
                }
            }
        }
        else {
            EditorUI::ccTouchEnded(touch, event);
        }
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
        auto bubble = Bubble<ObjTransformed>();
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            Bubble<ObjTransformed>::push(
                obj,
                Transform { m_fields->originalScales.at(i).first, obj->getPosition() },
                Transform<float>::zero(),
                Transform { m_fields->originalScales.at(i).second, obj->getScale() },
                Transform<bool>::zero(),
                Transform<bool>::zero()
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
        auto bubble = Bubble<ObjTransformed>();
        for (auto& [obj, pos, from] : scales) {
            Bubble<ObjTransformed>::push(
                obj,
                Transform { pos, obj->getPosition() },
                Transform<float>::zero(),
                Transform { from, obj->m_scale },
                Transform<bool>::zero(),
                Transform<bool>::zero()
            );
        }
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> angles;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            angles.push_back({ obj, obj->getPosition(), obj->m_rotation });
        }
        BLOCKED_CALL(EditorUI::rotateObjects(objs, angle, center));
        auto bubble = Bubble<ObjTransformed>();
        for (auto& [obj, pos, from] : angles) {
            Bubble<ObjTransformed>::push(
                obj,
                Transform { pos, obj->getPosition() },
                Transform { from, obj->m_rotation },
                Transform<float>::zero(),
                Transform<bool>::zero(),
                Transform<bool>::zero()
            );
        }
    }

    void flipObjectsX(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), false });
        }
        BLOCKED_CALL(EditorUI::flipObjectsX(objs));
        auto bubble = Bubble<ObjTransformed>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjTransformed>::push(
                obj,
                Transform { pos, obj->getPosition() },
                Transform<float>::zero(),
                Transform<float>::zero(),
                Transform { from, true },
                Transform<bool>::zero()
            );
        }
        m_fields->freeMoveFlippedX ^= 1;
    }

    void flipObjectsY(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), false });
        }
        BLOCKED_CALL(EditorUI::flipObjectsY(objs));
        auto bubble = Bubble<ObjTransformed>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjTransformed>::push(
                obj,
                Transform { pos, obj->getPosition() },
                Transform<float>::zero(),
                Transform<float>::zero(),
                Transform<bool>::zero(),
                Transform { from, true }
            );
        }
        m_fields->freeMoveFlippedY ^= 1;
    }

    void alignObjects(CCArray* objs, bool y) {
        auto bubble = Bubble<ObjTransformed>();
        EditorUI::alignObjects(objs, y);
    }

    void onPlaytest(CCObject* sender) {
        BLOCKED_CALL(EditorUI::onPlaytest(sender));
    }
};

class $modify(GameObject) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("GameObject::selectObject", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("GameObject::deselectObject", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("GameObject::removeFromGroup", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("GameObject::addToGroup", TRACKING_HOOK_PRIORITY);
    }

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
    static void onModify(auto& self) {
        (void)self.setHookPriority("CustomizeObjectLayer::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("CustomizeObjectLayer::onClose", TRACKING_HOOK_PRIORITY);
    }

    std::vector<ObjColorState> prev;

    bool init(GameObject* target, CCArray* targets) {
        if (!CustomizeObjectLayer::init(target, targets))
            return false;

        for (auto obj : iterTargets(target, targets)) {
            m_fields->prev.push_back(ObjColorState::from(obj));
        }
        
        return true;
    }

    void onClose(CCObject* sender) {
        auto bubble = Bubble<ObjColored>();
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            auto state = ObjColorState::from(obj);
            if (m_fields->prev.at(i) != state) {
                Bubble<ObjColored>::push(
                    obj, Transform { m_fields->prev.at(i), state }
                );
            }
            i += 1;
        }
        CustomizeObjectLayer::onClose(sender);
    }
};

struct $modify(ColorSelectPopup) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("ColorSelectPopup::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("ColorSelectPopup::closeColorSelect", TRACKING_HOOK_PRIORITY);
    }

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
        if (m_colorAction) {
            auto state = ColorState::from(m_colorAction);
            if (m_fields->state != state) {
                ColorChannelEvent(
                    m_colorAction->m_colorID,
                    Transform { m_fields->state, state }
                ).post();
            }
        }
        ColorSelectPopup::closeColorSelect(sender);
    }
};

class $modify(SetGroupIDLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("SetGroupIDLayer::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("SetGroupIDLayer::onAddGroup", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("SetGroupIDLayer::onClose", TRACKING_HOOK_PRIORITY);
    }

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
        SetGroupIDLayer::onClose(sender);
    }
};

using States = std::vector<std::pair<std::optional<TriggerState>, std::optional<SpecialState>>>;

static std::unordered_map<CCLayerColor*, States> ALERT_STATES {};

class $modify(CCLayerColor) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("CCLayerColor::initWithColor", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("CCLayerColor::destructor", TRACKING_HOOK_PRIORITY);
    }

    // mfw no fields on cocos classes yet

    bool initWithColor(ccColor4B const& color) {
        if (!CCLayerColor::initWithColor(color))
            return false;
        
        if (
            CCDirector::get()->getRunningScene()->getObjType() == CCObjectType::LevelEditorLayer &&
            LevelEditorLayer::get() && EditorUI::get()
        ) {
            auto ui = EditorUI::get();
            for (auto obj : iterTargets(ui->m_selectedObject, ui->m_selectedObjects)) {
                if (auto eobj = typeinfo_cast<EffectGameObject*>(obj)) {
                    ALERT_STATES[this].emplace_back(TriggerState::from(eobj), SpecialState::from(eobj));
                }
                else {
                    ALERT_STATES[this].emplace_back(std::nullopt, SpecialState::from(obj));
                }
            }
        }

        return true;
    }
    
    void destructor() {
        if (auto ui = EditorUI::get()) {
            if (ALERT_STATES.contains(this) && ALERT_STATES.at(this).size()) {
                size_t i = 0;
                auto bubble1 = Bubble<TriggerPropsChanged>();
                auto bubble2 = Bubble<SpecialPropsChanged>();
                for (auto obj : iterTargets(ui->m_selectedObject, ui->m_selectedObjects)) {
                    auto prev = ALERT_STATES.at(this).at(i++);
                    if (auto eobj = typeinfo_cast<EffectGameObject*>(obj)) {
                        auto state = TriggerState::from(eobj);
                        if (prev.first && state) {
                            if (prev.first.value().props != state.value().props) {
                                Bubble<TriggerPropsChanged>::push(
                                    eobj, Transform { prev.first.value(), state.value() }
                                );
                            }
                        }
                    }
                    auto state = SpecialState::from(obj);
                    if (prev.second && state) {
                        if (prev.second.value().props != state.value().props) {
                            Bubble<SpecialPropsChanged>::push(
                                obj, Transform { prev.second.value(), state.value() }
                            );
                        }
                    }
                }
            }
            ALERT_STATES.erase(this);
        }
        CCLayerColor::~CCLayerColor();
    }
};

class $modify(LevelSettingsLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelSettingsLayer::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("LevelSettingsLayer::onClose", TRACKING_HOOK_PRIORITY);
    }

    LevelSettingsState state;

    bool init(LevelSettingsObject* obj, LevelEditorLayer* lel) {
        if (!LevelSettingsLayer::init(obj, lel))
            return false;
        
        m_fields->state = LevelSettingsState::from(obj);
        
        return true;
    }

    void onClose(CCObject* sender) {
        if (m_settingsObject->m_startsWithStartPos) {
            // m_editorLayer is null wtf
            if (auto sel = typeinfo_cast<StartPosObject*>(EditorUI::get()->m_selectedObject)) {
                auto state = LevelSettingsState::from(sel->m_levelSettings);
                if (m_fields->state != state) {
                    Bubble<StartPosChanged>::push(sel, Transform { m_fields->state, state });
                }
            }
        }
        else {
            auto state = LevelSettingsState::from(LevelEditorLayer::get()->m_levelSettings);
            if (m_fields->state != state) {
                LevelSettingsChanged(Transform { m_fields->state, state }).post();
            }
        }
        LevelSettingsLayer::onClose(sender);
    }
};
