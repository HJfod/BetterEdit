#include <Geode/utils/cocos.hpp>
#include <tracking/Tracking.hpp>
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
#include <objects/ObjectType.hpp>

using namespace geode::prelude;
using namespace better_edit;

std::vector<Collector*> EditorEvent::s_groupCollection = {};

static std::unordered_set<ObjectKey> ALL_KEYS = {};

void better_edit::addObjectKey(int key) {
    ALL_KEYS.insert(static_cast<ObjectKey>(key));
}

void better_edit::removeObjectKey(int key) {
    ALL_KEYS.erase(static_cast<ObjectKey>(key));
}

StateValue StateValue::from(GameObject* obj, ObjectKey key) {
    auto res = StateValue();
    res.m_key = key;
    // todo
}

void StateValue::assign(GameObject* obj) const {
    switch (m_key) {
        case ObjectKey::X: {
            
        } break;
    }
}

ObjectKey StateValue::getKey() const {
    return m_key;
}

StateValue::List StateValue::all(GameObject* obj) {
    auto res = StateValue::List();
    res.reserve(ALL_KEYS.size());
    for (auto& key : ALL_KEYS) {
        res.push_back(StateValue::from(obj, key));
    }
    return res;
}

bool StateValue::operator==(StateValue const& other) const {
    return other.m_key == m_key && other.m_value == m_value;
}

bool StateValue::operator!=(StateValue const& other) const {
    return other.m_key != m_key || other.m_value != m_value;
}

bool StateValue::isSameState(StateValue const& other) const {
    return other.m_key == m_key && other.m_value.index() == m_value.index();
}

ChangedState::ChangedState(
    StateValue const& from,
    StateValue const& to
) : from(from), to(to) {
    if (from.isSameState(to)) {
        throw std::runtime_error("ChangedState::ChangedState: `from` and `to` don't refer to the same state");
    }
}

ObjectKeyMap ObjectKeyMap::from(GameObject* obj, StateValue::List const& previous) {
    auto map = ObjectKeyMap();
    map.m_object = obj;
    for (auto& p : previous) {
        auto n = StateValue::from(obj, p.getKey());
        if (n != p) {
            map.insert(p.getKey(), { p, n });
        }
    }
    return map;
}

bool ObjectKeyMap::contains(ObjectKey key) const {
    for (auto& change : m_changes) {
        if (change.from.getKey() == key) {
            return true;
        }
    }
    return false;
}

void ObjectKeyMap::insert(ObjectKey key, ChangedState const& state) {
    if (!this->contains(key)) {
        m_changes.push_back(state);
    }
}

size_t ObjectKeyMap::size() const {
    return m_changes.size();
}

void EditorEvent::post() {
    if (s_groupCollection.size()) {
        s_groupCollection.back()->push(this);
    } else {
        Event::post();
    }
}

GroupedEditorEvent GroupedEditorEvent::from(
    std::vector<ClonedPtr<EditorEvent>> const& events,
    std::string const& name
) {
    auto ret = GroupedEditorEvent();
    ret.m_name = name;
    for (auto& ev : events) {
        ret.m_events.push_back(ev);
    }
    return ret;
}

std::string GroupedEditorEvent::getName() const {
    return m_name;
}

EditorEvent* GroupedEditorEvent::clone() const {
    return new GroupedEditorEvent(*this);
}

void GroupedEditorEvent::undo() const {
    for (auto& event : ranges::reverse(m_events)) {
        event->undo();
    }
}

void GroupedEditorEvent::redo() const {
    for (auto& event : m_events) {
        event->redo();
    }
}

ObjectsPlacedEvent ObjectsPlacedEvent::from(std::vector<GameObject*> const& objs) {
    auto ev = ObjectsPlacedEvent();
    for (auto& obj : objs) {
        ev.m_objects.push_back({ obj, obj->getPosition() });
    }
    return ev;
}

std::string ObjectsPlacedEvent::getName() const {
    return fmt::format("Placed {} objects", m_objects.size());
}

EditorEvent* ObjectsPlacedEvent::clone() const {
    return new ObjectsPlacedEvent(*this);
}

void ObjectsPlacedEvent::undo() const {
    auto _ = Collect();
    for (auto& [obj, _] : m_objects) {
        LevelEditorLayer::get()->removeObjectFromSection(obj);
        LevelEditorLayer::get()->removeSpecial(obj);
        EditorUI::get()->deselectObject(obj);
        obj->deactivateObject(true);
    }
}

void ObjectsPlacedEvent::redo() const {
    auto _ = Collect();
    for (auto& [obj, pos] : m_objects) {
        LevelEditorLayer::get()->addToSection(obj);
        LevelEditorLayer::get()->addSpecial(obj);
        EditorUI::get()->moveObject(obj, pos - obj->getPosition());
        EditorUI::get()->selectObject(obj, true);
    }
}

void ObjectsPlacedEvent::merge(ObjectsPlacedEvent const& other) {
    ranges::push(m_objects, other.m_objects);
}

ObjectsRemovedEvent ObjectsRemovedEvent::from(std::vector<GameObject*> const& objs) {
    auto ev = ObjectsRemovedEvent();
    for (auto& obj : objs) {
        ev.m_objects.push_back(obj);
    }
    return ev;
}

std::string ObjectsRemovedEvent::getName() const {
    return fmt::format("Deleted {} objects", m_objects.size());
}

EditorEvent* ObjectsRemovedEvent::clone() const {
    return new ObjectsRemovedEvent(*this);
}

void ObjectsRemovedEvent::undo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
        LevelEditorLayer::get()->addToSection(obj);
        LevelEditorLayer::get()->addSpecial(obj);
        EditorUI::get()->selectObject(obj, true);
    }
}

void ObjectsRemovedEvent::redo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
        LevelEditorLayer::get()->removeObjectFromSection(obj);
        LevelEditorLayer::get()->removeSpecial(obj);
        EditorUI::get()->deselectObject(obj);
        obj->deactivateObject(true);
    }
}

void ObjectsRemovedEvent::merge(ObjectsRemovedEvent const& other) {
    ranges::push(m_objects, other.m_objects);
}

ObjectsSelectedEvent ObjectsSelectedEvent::from(std::vector<GameObject*> const& objs) {
    auto ev = ObjectsSelectedEvent();
    for (auto& obj : objs) {
        ev.m_objects.push_back(obj);
    }
    return ev;
}

std::string ObjectsSelectedEvent::getName() const {
    return fmt::format("Selected {} objects", m_objects.size());
}

EditorEvent* ObjectsSelectedEvent::clone() const {
    return new ObjectsSelectedEvent(*this);
}

void ObjectsSelectedEvent::undo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
        EditorUI::get()->deselectObject(obj);
    }
}

void ObjectsSelectedEvent::redo() const {
    auto _ = Collect();
    auto arr = EditorUI::get()->getSelectedObjects();
    for (auto& obj : m_objects) {
        arr->addObject(obj);
    }
    EditorUI::get()->selectObjects(arr, false);
}

void ObjectsSelectedEvent::merge(ObjectsSelectedEvent const& other) {
    ranges::push(m_objects, other.m_objects);
}

ObjectsDeselectedEvent ObjectsDeselectedEvent::from(std::vector<GameObject*> const& objs) {
    auto ev = ObjectsDeselectedEvent();
    for (auto& obj : objs) {
        ev.m_objects.push_back(obj);
    }
    return ev;
}

std::string ObjectsDeselectedEvent::getName() const {
    return fmt::format("Deselected {} objects", m_objects.size());
}

EditorEvent* ObjectsDeselectedEvent::clone() const {
    return new ObjectsDeselectedEvent(*this);
}

void ObjectsDeselectedEvent::undo() const {
    auto _ = Collect();
    auto arr = EditorUI::get()->getSelectedObjects();
    for (auto& obj : m_objects) {
        arr->addObject(obj);
    }
    EditorUI::get()->selectObjects(arr, false);
}

void ObjectsDeselectedEvent::redo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
        EditorUI::get()->deselectObject(obj);
    }
}

void ObjectsDeselectedEvent::merge(ObjectsDeselectedEvent const& other) {
    ranges::push(m_objects, other.m_objects);
}

ObjectsEditedEvent ObjectsEditedEvent::from(std::vector<ObjectKeyMap> const& objs) {
    auto ev = ObjectsEditedEvent();
    ev.m_objects = objs;
    return ev;
}

std::string ObjectsEditedEvent::getName() const {
    return fmt::format("Edited {} objects", m_objects.size());
}

EditorEvent* ObjectsEditedEvent::clone() const {
    return new ObjectsEditedEvent(*this);
}

void ObjectsEditedEvent::undo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
    }
}

void ObjectsEditedEvent::redo() const {
    auto _ = Collect();
    for (auto& obj : m_objects) {
    }
}

void ObjectsEditedEvent::merge(ObjectsEditedEvent const& other) {
    ranges::push(m_objects, other.m_objects);
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}
