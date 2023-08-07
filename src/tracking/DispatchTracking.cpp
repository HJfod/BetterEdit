#include <tracking/Tracking.hpp>
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

using namespace geode::prelude;
using namespace better_edit;

static int TRACKING_HOOK_PRIORITY = 999;

template <class F, class... Args>
void detectEdits(GameObject* obj, F func, Args... args) {
    StateValue::List state = StateValue::all(obj);
    (obj->*func)(args...);
    ObjectsEditedEvent::from({ ObjectKeyMap::from(obj, state) }).post();
}

template <class F, class... Args>
void detectEdits(F func, CCArray* objs, Args... args) {
    std::vector<StateValue::List> states;
    for (auto& obj : CCArrayExt<GameObject>(objs)) {
        states.push_back(StateValue::all(obj));
    }
    func(objs, args...);
    size_t i = 0;
    std::vector<ObjectKeyMap> res;
    for (auto& obj : CCArrayExt<GameObject>(objs)) {
        res.push_back(ObjectKeyMap::from(obj, states.at(i++)));
    }
    ObjectsEditedEvent::from(res).post();
}

template <class F>
void detectEdits(F func, GameObject* target, CCArray* targets) {
    std::vector<StateValue::List> states;
    for (auto& obj : iterTargets(target, targets)) {
        states.push_back(StateValue::all(obj));
    }
    func(target, targets);
    size_t i = 0;
    std::vector<ObjectKeyMap> res;
    for (auto& obj : iterTargets(target, targets)) {
        res.push_back(ObjectKeyMap::from(obj, states.at(i++)));
    }
    ObjectsEditedEvent::from(res).post();
}

class $modify(EditorPauseLayer) {
    void saveLevel() {
        auto _ = Collect();
        EditorPauseLayer::saveLevel();
    }

    void onSaveAndPlay(CCObject* sender) {
        auto _ = Collect();
        EditorPauseLayer::onSaveAndPlay(sender);
    }

    void onExitEditor(CCObject* sender) {
        auto _ = Collect();
        EditorPauseLayer::onExitEditor(sender);
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level) {
        auto _ = Collect();
        return PlayLayer::init(level);
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
        {
            auto _ = Collect();
            LevelEditorLayer::addSpecial(obj);
        }
        ObjectsPlacedEvent::from({ obj }).post();
    }

    CCArray* createObjectsFromString(gd::string str, bool undo) {
        CCArray* objs;
        {
            auto _ = Collect();
            objs = LevelEditorLayer::createObjectsFromString(str, undo);
        }
        ObjectsPlacedEvent::from(ccArrayToVector<GameObject*>(objs)).post();
        return objs;
    }

    void createObjectsFromSetup(gd::string str) {
        auto _ = Collect();
        LevelEditorLayer::createObjectsFromSetup(str);
    }

    void removeSpecial(GameObject* obj) {
        {
            auto _ = Collect();
            LevelEditorLayer::removeSpecial(obj);
        }
        ObjectsRemovedEvent::from({ obj }).post();
    }

    void pasteAtributeState(GameObject* target, CCArray* targets) {
        detectEdits(&LevelEditorLayer::pasteAtributeState, target, targets);
    }

    void pasteColorState(GameObject* target, CCArray* targets) {
        detectEdits(&LevelEditorLayer::pasteColorState, target, targets);
    }
};

class $modify(EditorUI) {
    std::vector<StateValue::List> originalScales;
    std::vector<StateValue::List> freeMoveStart;
    std::unique_ptr<Collect<ObjectsEditedEvent>> freeMoveCollect;

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
        auto c = Collect("Copied Objects");
        EditorUI::onDuplicate(sender);
        c.post();
    }

    bool onCreate() {
        // block selection events
        auto _ = Collect<ObjectsSelectedEvent>();
        auto __ = Collect<ObjectsDeselectedEvent>();
        return EditorUI::onCreate();
    }

    void onDelete(CCObject* sender) {
        auto _ = Collect<ObjectsDeselectedEvent>();
        EditorUI::onDelete(sender);
    }

    void onDeleteSelected(CCObject* sender) {
        auto _ = Collect<ObjectsDeselectedEvent>();
        auto c = Collect<ObjectsRemovedEvent>();
        EditorUI::onDeleteSelected(sender);
        c.post();
    }

    void deleteObject(GameObject* obj, bool filter) {
        {
            auto _ = Collect();
            EditorUI::deleteObject(obj, filter);
        }
        ObjectsRemovedEvent::from({ obj }).post();
    }

    void moveObject(GameObject* obj, CCPoint to) {
        auto from = StateValue::list(obj, ObjectKey::X, ObjectKey::Y);
        {
            auto _ = Collect();
            EditorUI::moveObject(obj, to);
        }
        ObjectsEditedEvent::from({ ObjectKeyMap::from(obj, from) }).post();
    }

    void moveObjectCall(EditCommand command) {
        auto _ = Collect<ObjectsEditedEvent>();
        EditorUI::moveObjectCall(command);
    }

    void transformObjectCall(EditCommand command) {
        auto _ = Collect<ObjectsEditedEvent>();
        EditorUI::transformObjectCall(command);
    }

    void transformObject(GameObject* obj, EditCommand command, bool snap) {
        auto from = StateValue::all(obj);
        switch (command) {
            case EditCommand::RotateCCW:  case EditCommand::RotateCCW45:
            case EditCommand::RotateCW:   case EditCommand::RotateCW45:
            case EditCommand::RotateSnap: case EditCommand::RotateFree: {
                {
                    auto _ = Collect();
                    EditorUI::transformObject(obj, command, snap);
                }
            } break;

            case EditCommand::FlipX: {
                {
                    auto _ = Collect();
                    EditorUI::transformObject(obj, command, snap);
                }
            } break;

            case EditCommand::FlipY: {
                {
                    auto _ = Collect();
                    EditorUI::transformObject(obj, command, snap);
                }
            } break;

            default: {
                EditorUI::transformObject(obj, command, snap);
            } break;
        }
        ObjectsEditedEvent::from({ ObjectKeyMap::from(obj, from) }).post();
    }

    void deselectAll() {
        auto c = Collect<ObjectsDeselectedEvent>();
        EditorUI::deselectAll();
        c.post();
    }

    void selectObjects(CCArray* objs, bool filter) {
        auto c = Collect<ObjectsSelectedEvent>();
        EditorUI::selectObjects(objs, filter);
        c.post();
    }
 
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (EditorUI::ccTouchBegan(touch, event)) {
            m_fields->freeMoveStart = {};
            for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
                m_fields->freeMoveStart.push_back(StateValue::all(obj));
            }
            return true;
        }
        return false;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        {
            auto _ = Collect<ObjectsEditedEvent>();
            EditorUI::ccTouchMoved(touch, event);
        }
        if (m_freeMovingObject && !m_fields->freeMoveCollect.get()) {
            m_fields->freeMoveCollect = std::make_unique<Collect<ObjectsEditedEvent>>("Free Moved Object(s)");
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (m_freeMovingObject) {
            {
                auto _ = Collect();
                EditorUI::ccTouchEnded(touch, event);
            }
            if (m_fields->freeMoveCollect) {
                m_fields->freeMoveCollect = nullptr;
            }
            std::vector<ObjectKeyMap> events {};
            size_t i = 0;
            for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
                events.push_back(ObjectKeyMap::from(obj, m_fields->freeMoveStart.at(i++)));
            }
            ObjectsEditedEvent::from(events).post();
        }
        else {
            EditorUI::ccTouchEnded(touch, event);
        }
    }

    void scaleChangeBegin() {
        m_fields->originalScales = {};
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            m_fields->originalScales.emplace_back(StateValue::all(obj));
        }
        {
            auto _ = Collect();
            EditorUI::scaleChangeBegin();
        }
    }

    void scaleChanged(float scale) {
        auto _ = Collect();
        EditorUI::scaleChanged(scale);
    }

    void scaleChangeEnded() {
        {
            auto _ = Collect();
            EditorUI::scaleChangeEnded();
        }
        size_t i = 0;
        std::vector<ObjectKeyMap> events {};
        for (auto obj : iterTargets(m_selectedObject, m_selectedObjects)) {
            events.push_back(ObjectKeyMap::from(obj, m_fields->originalScales.at(i++)));
        }
        ObjectsEditedEvent::from(events).post();
    }

    void scaleObjects(CCArray* objs, float scale, CCPoint center) {
        detectEdits(&EditorUI::scaleObjects, objs, scale, center);
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        detectEdits(&EditorUI::rotateObjects, objs, angle, center);
    }

    void flipObjectsX(CCArray* objs) {
        detectEdits(&EditorUI::flipObjectsX, objs);
    }

    void flipObjectsY(CCArray* objs) {
        detectEdits(&EditorUI::flipObjectsY, objs);
    }

    void alignObjects(CCArray* objs, bool y) {
        auto c = Collect();
        EditorUI::alignObjects(objs, y);
        c.post();
    }

    void onPlaytest(CCObject* sender) {
        auto _ = Collect();
        EditorUI::onPlaytest(sender);
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
        ObjectsSelectedEvent::from({ this }).post();
    }

    void deselectObject() {
        GameObject::deselectObject();
        ObjectsDeselectedEvent::from({ this }).post();
    }

    void removeFromGroup(int group) {
        detectEdits(this, &GameObject::removeFromGroup, group);
    }

    void addToGroup(int group) {
        detectEdits(this, &GameObject::addToGroup, group);
    }
};

class $modify(CustomizeObjectLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("CustomizeObjectLayer::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("CustomizeObjectLayer::onClose", TRACKING_HOOK_PRIORITY);
    }

    std::vector<StateValue::List> prev;

    bool init(GameObject* target, CCArray* targets) {
        if (!CustomizeObjectLayer::init(target, targets))
            return false;

        for (auto obj : iterTargets(target, targets)) {
            m_fields->prev.push_back(StateValue::all(obj));
        }
        
        return true;
    }

    void onClose(CCObject* sender) {
        std::vector<ObjectKeyMap> events {};
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            events.push_back(ObjectKeyMap::from(obj, m_fields->prev.at(i++)));
        }
        ObjectsEditedEvent::from(events).post();
        CustomizeObjectLayer::onClose(sender);
    }
};

// struct $modify(ColorSelectPopup) {
//     static void onModify(auto& self) {
//         (void)self.setHookPriority("ColorSelectPopup::init", TRACKING_HOOK_PRIORITY);
//         (void)self.setHookPriority("ColorSelectPopup::closeColorSelect", TRACKING_HOOK_PRIORITY);
//     }

//     ColorState state;

//     bool init(EffectGameObject* target, CCArray* targets, ColorAction* action) {
//         if (!ColorSelectPopup::init(target, targets, action))
//             return false;

//         if (action) {
//             m_fields->state = ColorState::from(action);
//         }
        
//         return true;
//     }

//     void closeColorSelect(CCObject* sender) {
//         if (m_colorAction) {
//             auto state = ColorState::from(m_colorAction);
//             if (m_fields->state != state) {
//                 ColorChannelEvent(
//                     m_colorAction->m_colorID,
//                     Transform { m_fields->state, state }
//                 ).post();
//             }
//         }
//         ColorSelectPopup::closeColorSelect(sender);
//     }
// };

class $modify(SetGroupIDLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("SetGroupIDLayer::init", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("SetGroupIDLayer::onAddGroup", TRACKING_HOOK_PRIORITY);
        (void)self.setHookPriority("SetGroupIDLayer::onClose", TRACKING_HOOK_PRIORITY);
    }

    std::vector<StateValue::List> prev;

    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        for (auto obj : iterTargets(obj, objs)) {
            m_fields->prev.push_back(StateValue::all(obj));
        }

        return true;
    }

    void onAddGroup(CCObject* sender) {
        auto _ = Collect();
        SetGroupIDLayer::onAddGroup(sender);
    }

    void onClose(CCObject* sender) {
        std::vector<ObjectKeyMap> events {};
        size_t i = 0;
        for (auto obj : iterTargets(m_targetObject, m_targetObjects)) {
            events.push_back(ObjectKeyMap::from(obj, m_fields->prev.at(i++)));
        }
        ObjectsEditedEvent::from(events).post();
        SetGroupIDLayer::onClose(sender);
    }
};

static std::unordered_map<CCLayerColor*, std::vector<StateValue::List>> ALERT_STATES {};

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
                ALERT_STATES[this].push_back(StateValue::all(obj));
            }
        }

        return true;
    }
    
    void destructor() {
        if (auto ui = EditorUI::get()) {
            if (ALERT_STATES.contains(this) && ALERT_STATES.at(this).size()) {
                std::vector<ObjectKeyMap> events {};
                size_t i = 0;
                for (auto obj : iterTargets(ui->m_selectedObject, ui->m_selectedObjects)) {
                    events.push_back(ObjectKeyMap::from(obj, ALERT_STATES.at(this).at(i++)));
                }
                ObjectsEditedEvent::from(events).post();
            }
            ALERT_STATES.erase(this);
        }
        CCLayerColor::~CCLayerColor();
    }
};

// class $modify(LevelSettingsLayer) {
//     static void onModify(auto& self) {
//         (void)self.setHookPriority("LevelSettingsLayer::init", TRACKING_HOOK_PRIORITY);
//         (void)self.setHookPriority("LevelSettingsLayer::onClose", TRACKING_HOOK_PRIORITY);
//     }

//     LevelSettingsState state;

//     bool init(LevelSettingsObject* obj, LevelEditorLayer* lel) {
//         if (!LevelSettingsLayer::init(obj, lel))
//             return false;
        
//         m_fields->state = LevelSettingsState::from(obj);
        
//         return true;
//     }

//     void onClose(CCObject* sender) {
//         if (m_settingsObject->m_startsWithStartPos) {
//             // m_editorLayer is null wtf
//             if (auto sel = typeinfo_cast<StartPosObject*>(EditorUI::get()->m_selectedObject)) {
//                 auto state = LevelSettingsState::from(sel->m_levelSettings);
//                 if (m_fields->state != state) {
//                     Bubble<StartPosChanged>::push(sel, Transform { m_fields->state, state });
//                 }
//             }
//         }
//         else {
//             auto state = LevelSettingsState::from(LevelEditorLayer::get()->m_levelSettings);
//             if (m_fields->state != state) {
//                 LevelSettingsChanged(Transform { m_fields->state, state }).post();
//             }
//         }
//         LevelSettingsLayer::onClose(sender);
//     }
// };
