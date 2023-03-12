#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

using ObjRefs = std::vector<Ref<GameObject>>;
template <class... T>
using ObjRefTuples = std::vector<std::tuple<Ref<GameObject>, T...>>;

template <class T>
struct Transform {
    T from, to;
    Transform(T const& from, T const& to) : from(from), to(to) {}
};

template <class T>
using OptTransform = std::optional<Transform<T>>;

std::string toDiffString(Ref<GameObject> obj);
std::string toDiffString(CCPoint const& point);
std::string toDiffString(float value);
std::string toDiffString(bool value);

template <class T>
std::string toDiffString(Transform<T> const& value) {
    return toDiffString(value.from) + "," + toDiffString(value.to);
}

template <class T>
std::string toDiffString(OptTransform<T> const& value) {
    return value ? toDiffString(value.value()) : "_";
}

template <class... T, std::size_t... Is>
std::string toDiffString(std::tuple<T...> const& tuple, std::index_sequence<Is...>) {
    std::string ret = "";
    ((ret += (Is == 0 ? "" : ",") + toDiffString(std::get<Is>(tuple))), ...);
    return ret;
}

template <class... T>
std::string toDiffString(std::tuple<T...> const& tuple) {
    return toDiffString(tuple, std::index_sequence_for<T...> {});
}

template <class T>
std::string toDiffString(std::vector<T> const& objs) {
    std::string res = "";
    bool first = true;
    for (auto& obj : objs) {
        if (!first) {
            res += ";";
        }
        first = false;
        res += toDiffString(obj);
    }
    return res;
}

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

struct ObjectTransformedEvent : public ObjectEvent {
    Transform<CCPoint> pos;
    OptTransform<float> scale;
    OptTransform<float> angle;
    OptTransform<bool> flipX;
    OptTransform<bool> flipY;
    ObjectTransformedEvent(
        Ref<GameObject> obj,
        Transform<CCPoint> const& pos,
        OptTransform<float> const& scale,
        OptTransform<float> const& angle,
        OptTransform<bool> const& flipX,
        OptTransform<bool> const& flipY
    );
    static ObjectTransformedEvent moved(
        Ref<GameObject> obj, Transform<CCPoint> const& pos
    );
    static ObjectTransformedEvent scaled(
        Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> const& scale
    );
    static ObjectTransformedEvent rotated(
        Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> const& angle
    );
    static ObjectTransformedEvent flippedX(
        Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip
    );
    static ObjectTransformedEvent flippedY(
        Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip
    );
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
