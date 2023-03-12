#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

std::string objectHash(GameObject* obj);

struct EditorEvent : public Event {
    virtual std::string toDiffString() const = 0;
    virtual void undo() const = 0;
    virtual void redo() const = 0;
    virtual EditorEvent* clone() const = 0;
    std::unique_ptr<EditorEvent> unique() const;
};

struct ObjectEvent : public EditorEvent {
    Ref<GameObject> obj;
    ObjectEvent(Ref<GameObject> obj);
};

struct ObjectPlacedEvent : public ObjectEvent {
    CCPoint pos;
    ObjectPlacedEvent(Ref<GameObject> obj, CCPoint const& pos);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectRemovedEvent : public ObjectEvent {
    ObjectRemovedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectMovedEvent : public ObjectEvent {
    CCPoint pos;
    ObjectMovedEvent(Ref<GameObject> obj, CCPoint const& pos);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectRotatedEvent : public ObjectEvent {
    float angle;
    ObjectRotatedEvent(Ref<GameObject> obj, float angle);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectScaledEvent : public ObjectEvent {
    float scale;
    ObjectScaledEvent(Ref<GameObject> obj, float scale);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectFlippedXEvent : public ObjectEvent {
    bool flipped;
    ObjectFlippedXEvent(Ref<GameObject> obj, bool flipped);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectFlippedYEvent : public ObjectEvent {
    bool flipped;
    ObjectFlippedYEvent(Ref<GameObject> obj, bool flipped);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectSelectedEvent : public ObjectEvent {
    ObjectSelectedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectDeselectedEvent : public ObjectEvent {
    ObjectDeselectedEvent(Ref<GameObject> obj);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

template <class Ev>
    requires std::is_base_of_v<ObjectEvent, Ev>
struct MultiObjectEvent : public EditorEvent {
    std::vector<Ev> events;
    MultiObjectEvent(std::vector<Ev> const& events) : events(events) {}
    std::string toDiffString() const override {
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

    EditorEvent* clone() const override {
        return new MultiObjectEvent(events);
    }

    void undo() const override {
        for (auto& ev : events) {
            ev.undo();
        }
    }

    void redo() const override {
        for (auto& ev : events) {
            ev.redo();
        }
    }
};

struct EditorFilter : public EventFilter<EditorEvent> {
public:
    using Callback = void(EditorEvent*);

    ListenerResult handle(utils::MiniFunction<Callback> fn, EditorEvent* event);
    EditorFilter() = default;
};
