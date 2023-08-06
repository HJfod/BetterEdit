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

bool EditorEvent::s_blockEvents = false;

ChangedState::ChangedState(
    StateValue const& from,
    StateValue const& to
) : from(from), to(to) {}

BlockEventsHandle::BlockEventsHandle() {
    EditorEvent::s_blockEvents = true;
}

BlockEventsHandle::~BlockEventsHandle() {
    EditorEvent::s_blockEvents = false;
}

EditorEvent::EditorEvent(Type const& type) : m_event(type) {}

BlockEventsHandle EditorEvent::block() {
    return BlockEventsHandle();
}

void EditorEvent::post(Type const& type) {
    if (!s_blockEvents) {
        EditorEvent(type).Event::post();
    }
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}
