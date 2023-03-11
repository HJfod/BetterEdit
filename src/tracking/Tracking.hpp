#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

struct ObjectEvent {
    Ref<GameObject> obj;
    ObjectEvent(Ref<GameObject> obj);
    virtual std::string toDiffString() const = 0;
};

struct ObjectPlacedEvent : public ObjectEvent {
    CCPoint pos;
    ObjectPlacedEvent(Ref<GameObject> obj, CCPoint const& pos);
    std::string toDiffString() const override;
};

struct ObjectRemovedEvent : public ObjectEvent {
    ObjectRemovedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
};

struct ObjectMovedEvent : public ObjectEvent {
    CCPoint pos;
    ObjectMovedEvent(Ref<GameObject> obj, CCPoint const& pos);
    std::string toDiffString() const override;
};

struct ObjectSelectedEvent : public ObjectEvent {
    ObjectSelectedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
};

struct ObjectDeselectedEvent : public ObjectEvent {
    ObjectDeselectedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
};

template <class Ev>
    requires std::is_base_of_v<ObjectEvent, Ev>
struct MultiObjectEvent {
    std::vector<Ev> events;
    MultiObjectEvent(std::vector<Ev> const& events) : events(events) {}
    std::string toDiffString() const {
        std::string res = "";
        bool first = true;
        for (auto& ev : events) {
            if (!first) {
                res += "\n";
            }
            first = false;
            res += ev.toDiffString();
        }
        return res;
    }
};

struct EditorEvent : public Event {
    std::variant<
        ObjectPlacedEvent,
        ObjectRemovedEvent,
        ObjectMovedEvent,
        ObjectSelectedEvent,
        ObjectDeselectedEvent,
        MultiObjectEvent<ObjectPlacedEvent>,
        MultiObjectEvent<ObjectRemovedEvent>,
        MultiObjectEvent<ObjectMovedEvent>,
        MultiObjectEvent<ObjectSelectedEvent>,
        MultiObjectEvent<ObjectDeselectedEvent>
    > kind;
    using Kind = decltype(kind);
    EditorEvent(Kind const& kind);
    std::string toDiffString() const;
};

struct EditorFilter : public EventFilter<EditorEvent> {
public:
    using Callback = void(EditorEvent*);

    ListenerResult handle(utils::MiniFunction<Callback> fn, EditorEvent* event);
    EditorFilter() = default;
};
