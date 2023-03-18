#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/Mod.hpp>
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

struct ColorState {
    ccColor3B color;
    float opacity;
    bool blending;
    int playerColor;
    int copyColorID;
    ccHSVValue copyHSV;

    static ColorState from(ColorAction* action);
    void to(ColorAction* action) const;
    bool operator==(ColorState const& other) const;
    bool operator!=(ColorState const& other) const;
};

struct ObjState {
    std::vector<short> groups;
    int editorLayer1;
    int editorLayer2;
    int zOrder;
    ZLayer zLayer;
    bool dontFade;
    bool dontEnter;
    bool groupParent;
    bool highDetail;

    static ObjState from(GameObject* obj);
    void to(GameObject* obj) const;
    bool operator==(ObjState const& other) const;
    bool operator!=(ObjState const& other) const;
};

struct BlockAll {
    static inline BlockAll* s_current = nullptr;
    static bool blocked();
    BlockAll();
    ~BlockAll();
};

std::string toDiffString(Ref<GameObject> obj);
std::string toDiffString(CCPoint const& point);
std::string toDiffString(bool value);
std::string toDiffString(ZLayer const& value);
std::string toDiffString(ccHSVValue const& value);
std::string toDiffString(ccColor3B const& value);
std::string toDiffString(ccColor4B const& value);
std::string toDiffString(ColorState const& value);
std::string toDiffString(ObjState const& value);
std::string toDiffString(std::vector<short> const& value);

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

struct Detail {
    std::string info;
    std::optional<std::string> icon;

    bool operator==(Detail const& other) const;
};

struct EditorEventData {
    virtual std::string toDiffString() const = 0;
    virtual void undo() const = 0;
    virtual void redo() const = 0;
    virtual EditorEventData* clone() const = 0;
    virtual std::vector<Detail> details() const = 0;
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
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "plus.png"_spr;
    static inline auto DESC_FMT = "Added {}";
};

struct ObjRemoved : public ObjEventData {
    inline ObjRemoved(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "neg.png"_spr;
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
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "move.png"_spr;
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
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "edit_ccwBtn_001.png";
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
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "scale.png"_spr;
    static inline auto DESC_FMT = "Scaled {}";
};

struct ObjFlipX : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<bool> flip;
    inline ObjFlipX(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip)
      : ObjEventData(obj), pos(pos), flip(flip) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "edit_flipXBtn_001.png";
    static inline auto DESC_FMT = "Flipped {} on the X-axis";
};

struct ObjFlipY : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<bool> flip;
    inline ObjFlipY(Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip)
      : ObjEventData(obj), pos(pos), flip(flip) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "edit_flipYBtn_001.png";
    static inline auto DESC_FMT = "Flipped {} on the Y-axis";
};

struct ObjColored : public ObjEventData {
    bool detail;
    Transform<int> channel;
    inline ObjColored(
        Ref<GameObject> obj,
        bool detail,
        Transform<int> const& channel
    ) : ObjEventData(obj),
        channel(channel),
        detail(detail) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "color.png"_spr;
    static inline auto DESC_FMT = "Colored {}";
};

struct ObjHSVChanged : public ObjEventData {
    bool detail;
    Transform<ccHSVValue> hsv;

    inline ObjHSVChanged(Ref<GameObject> obj, bool detail, Transform<ccHSVValue> hsv)
      : ObjEventData(obj), detail(detail), hsv(hsv) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "color.png"_spr;
    static inline auto DESC_FMT = "Changed HSV for {}";
};

struct ObjColorPasted : public ObjEventData {
    Transform<int> baseChannel;
    Transform<ccHSVValue> baseHSV;
    Transform<int> detailChannel;
    Transform<ccHSVValue> detailHSV;

    inline ObjColorPasted(
        Ref<GameObject> obj,
        Transform<int> const& baseChannel,
        Transform<ccHSVValue> const& baseHSV,
        Transform<int> const& detailChannel,
        Transform<ccHSVValue> const& detailHSV
    ) : ObjEventData(obj),
        baseChannel(baseChannel), baseHSV(baseHSV),
        detailChannel(detailChannel), detailHSV(detailHSV) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "color.png"_spr;
    static inline auto DESC_FMT = "Pasted Color to {}";
};

struct ObjPropsChanged : public ObjEventData {
    Transform<ObjState> state;

    inline ObjPropsChanged(Ref<GameObject> obj, Transform<ObjState> const& state)
      : ObjEventData(obj), state(state) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "GJ_hammerIcon_001.png";
    static inline auto DESC_FMT = "Changed {} Properties";
};

struct ObjSelected : public ObjEventData {
    inline ObjSelected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "select.png"_spr;
    static inline auto DESC_FMT = "Selected {}";
};

struct ObjDeselected : public ObjEventData {
    inline ObjDeselected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    static inline auto ICON_NAME = "deselect.png"_spr;
    static inline auto DESC_FMT = "Deselected {}";
};

struct EditorEvent : public Event, public EditorEventData {
    virtual std::string desc() const = 0;
    virtual CCNode* icon() const = 0;
    std::unique_ptr<EditorEvent> unique() const;
};

struct ColorChannelEvent : public EditorEvent {
    int channel;
    Transform<ColorState> state;

    inline ColorChannelEvent(int channel, Transform<ColorState> const& state)
      : channel(channel), state(state) {}
    
    std::string toDiffString() const override;
    void undo() const override;
    void redo() const override;
    EditorEventData* clone() const override;
    std::vector<Detail> details() const override;

    CCNode* icon() const override;
    std::string desc() const override;
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

    std::vector<Detail> details() const override {
        std::vector<Detail> res;
        for (auto& ev : events) {
            for (auto& detail : ev.details()) {
                if (!ranges::contains(res, detail)) {
                    res.push_back(detail);
                }
            }
        }
        return res;
    }

    CCNode* icon() const override {
        auto base = CCSprite::createWithSpriteFrameName("square_01_001.png");
        auto top = CCSprite::createWithSpriteFrameName(Ev::ICON_NAME);
        top->setPosition({ base->getContentSize().width - 5.f, 5.f });
        base->addChild(top);
        base->setScale(.5f);
        return base;
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
