#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

struct EditorEvent : public Event {
    virtual std::string toDiffString() const = 0;
    virtual void undo() const = 0;
    virtual void redo() const = 0;
    virtual EditorEvent* clone() const = 0;
    std::unique_ptr<EditorEvent> unique() const;
};

using ObjRefs = std::vector<Ref<GameObject>>;
template <class... T>
using ObjRefTuples = std::vector<std::tuple<Ref<GameObject>, T...>>;

std::string toDiffString(Ref<GameObject> obj);
std::string toDiffString(CCPoint const& point);
std::string toDiffString(float value);
std::string toDiffString(bool value);

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

struct ObjectPlacedEvent : public EditorEvent {
    ObjRefTuples<CCPoint> objs;
    ObjectPlacedEvent(ObjRefTuples<CCPoint> const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectRemovedEvent : public EditorEvent {
    ObjRefs objs;
    ObjectRemovedEvent(ObjRefs const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectMovedEvent : public EditorEvent {
    ObjRefTuples<CCPoint, CCPoint> objs;
    ObjectMovedEvent(ObjRefTuples<CCPoint, CCPoint> const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectRotatedEvent : public EditorEvent {
    ObjRefs objs;
    float from, to;
    ObjectRotatedEvent(ObjRefs const& objs, float from, float to);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectScaledEvent : public EditorEvent {
    ObjRefs objs;
    float from, to;
    ObjectScaledEvent(ObjRefs const& objs, float from, float to);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectFlippedXEvent : public EditorEvent {
    ObjRefTuples<bool> objs;
    ObjectFlippedXEvent(ObjRefTuples<bool> const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectFlippedYEvent : public EditorEvent {
    ObjRefTuples<bool> objs;
    ObjectFlippedYEvent(ObjRefTuples<bool> const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectSelectedEvent : public EditorEvent {
    ObjRefs objs;
    ObjectSelectedEvent(ObjRefs const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct ObjectDeselectedEvent : public EditorEvent {
    ObjRefs objs;
    ObjectDeselectedEvent(ObjRefs const& objs);
    std::string toDiffString() const override;
    EditorEvent* clone() const override;
    void undo() const override;
    void redo() const override;
};

struct EditorFilter : public EventFilter<EditorEvent> {
public:
    using Callback = void(EditorEvent*);

    ListenerResult handle(utils::MiniFunction<Callback> fn, EditorEvent* event);
    EditorFilter() = default;
};
