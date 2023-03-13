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
std::string toDiffString(bool value);

template <class T>
std::string toDiffString(T const& value) {
    return fmt::format("{}", value);
}

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

template <class... Args, std::size_t... Is>
std::string fmtDiffStringImpl(std::string_view const& str, Args&&... args, std::index_sequence<Is...>) {
    std::string ret { str };
    ret.push_back(' ');
    ((ret += (Is == 0 ? "" : ":") + toDiffString(args)), ...);
    return ret;
}

template <class... Args>
std::string fmtDiffString(std::string_view const& str, Args&&... args) {
    return fmtDiffStringImpl<Args...>(str, std::forward<Args>(args)..., std::index_sequence_for<Args...> {});
}

struct EditorEventData {
    virtual std::string toDiffString() const = 0;
    virtual void undo() const = 0;
    virtual void redo() const = 0;
    virtual EditorEventData* clone() const = 0;
};

struct ObjEventData : public EditorEventData {
    Ref<GameObject> obj;
    ObjEventData(Ref<GameObject> obj);
};

struct ObjPlaced : public ObjEventData {
    CCPoint pos;
    inline ObjPlaced(Ref<GameObject> obj, CCPoint const& pos)
      : ObjEventData(obj), pos(pos) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "add-obj.png"_spr;
    static inline auto DESC_FMT = "Added {}";
};

struct ObjRemoved : public ObjEventData {
    inline ObjRemoved(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "rem-obj.png"_spr;
    static inline auto DESC_FMT = "Removed {}";
};

struct ObjMoved : public ObjEventData {
    Transform<CCPoint> pos;
    inline ObjMoved(Ref<GameObject> obj, Transform<CCPoint> const& pos)
      : ObjEventData(obj), pos(pos) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "move-obj.png"_spr;
    static inline auto DESC_FMT = "Moved {}";
};

struct ObjRotated : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<float> angle;
    inline ObjRotated(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> angle)
      : ObjEventData(obj), pos(pos), angle(angle) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "rot-obj.png"_spr;
    static inline auto DESC_FMT = "Rotated {}";
};

struct ObjScaled : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<float> scale;
    inline ObjScaled(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> scale)
      : ObjEventData(obj), pos(pos), scale(scale) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "scale-obj.png"_spr;
    static inline auto DESC_FMT = "Scaled {}";
};

struct ObjFlipX : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<bool> flip;
    inline ObjFlipX(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> flip)
      : ObjEventData(obj), pos(pos), flip(flip) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "flipx-obj.png"_spr;
    static inline auto DESC_FMT = "Flipped {} on the X-axis";
};

struct ObjFlipY : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<bool> flip;
    inline ObjFlipY(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> flip)
      : ObjEventData(obj), pos(pos), flip(flip) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "flipy-obj.png"_spr;
    static inline auto DESC_FMT = "Flipped {} on the Y-axis";
};

struct ObjSelected : public ObjEventData {
    inline ObjSelected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "sel-obj.png"_spr;
    static inline auto DESC_FMT = "Selected {}";
};

struct ObjDeselected : public ObjEventData {
    inline ObjDeselected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;

    static inline auto ICON_NAME = "desel-obj.png"_spr;
    static inline auto DESC_FMT = "Deselected {}";
};

struct EditorEvent : public Event, public EditorEventData {
    virtual std::string desc() const = 0;
    virtual const char* icon() const = 0;
    std::unique_ptr<EditorEvent> unique() const;
};

template <class Ev>
    requires std::is_base_of_v<ObjEventData, Ev>
struct MultiObjEvent : public EditorEvent {
    std::vector<Ev> events;
    MultiObjEvent(std::vector<Ev> const& events) : events(events) {}
    std::string toDiffString() const override {
        std::string res = "";
        bool first = true;
        for (auto& ev : events) {
            if (!first) {
                res += "&";
            }
            first = false;
            res += ev.toDiffString();
        }
        return res;
    }

    EditorEventData* clone() const override {
        return new MultiObjEvent(events);
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

    const char* icon() const override {
        return Ev::ICON_NAME;
    }

    std::string desc() const override {
        return fmt::format(
            fmt::runtime(Ev::DESC_FMT),
            (events.size() == 1 ?
                "Object" :
                fmt::format("{} Objects", events.size()))
        );
    }
};

struct EditorFilter : public EventFilter<EditorEvent> {
public:
    using Callback = void(EditorEvent*);

    ListenerResult handle(utils::MiniFunction<Callback> fn, EditorEvent* event);
    EditorFilter() = default;
};
