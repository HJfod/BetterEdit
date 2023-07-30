#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/StartPosObject.hpp>

using namespace geode::prelude;

using ObjRefs = std::vector<Ref<GameObject>>;
template <class... T>
using ObjRefTuples = std::vector<std::tuple<Ref<GameObject>, T...>>;

template <class T>
struct Transform {
    T from, to;
    Transform(T const& from, T const& to) : from(from), to(to) {}
    static Transform zero() {
        return { T(), T() };
    }

    T delta() const {
        return to - from;
    }
    bool isZero() const {
        return to == from;
    }
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
    bool operator==(ColorState const& other) const = default;
};

struct ObjColorState {
    int base;
    ccHSVValue baseHSV;
    int detail;
    ccHSVValue detailHSV;
    bool glow;

    static ObjColorState from(GameObject* obj);
    void to(GameObject* obj) const;
    bool operator==(ObjColorState const& other) const = default;
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
    bool operator==(ObjState const& other) const = default;
};

struct SpecialState {
    struct CollisionBlock {
        int id;
        bool dynamic;
        bool operator==(CollisionBlock const&) const = default;
    };

    struct Counter {
        int id;
        bool operator==(Counter const&) const = default;
    };

    struct Saw {
        int degrees; // 0 is default
        bool disable;
        bool operator==(Saw const&) const = default;
    };

    struct OrbPadPortal {
        bool multiActivate;
        bool operator==(OrbPadPortal const&) const = default;
    };

    struct ToggleOrb {
        int targetGroupID;
        bool activateGroup;
        bool multiActivate;
        bool operator==(ToggleOrb const&) const = default;
    };

    struct Collectible {
        int pickupMode;
        int itemID;
        bool subtractCount;
        int groupID;
        bool enableGroup;
        bool operator==(Collectible const&) const = default;
    };

    struct Pulse {
        bool randomizeStart;
        float speed;
        bool operator==(Pulse const&) const = default;
    };

    std::variant<
        CollisionBlock, Counter, Saw, OrbPadPortal,
        ToggleOrb, Collectible, Pulse
    > props;

    static std::optional<SpecialState> from(GameObject* obj);
    void to(GameObject* obj) const;
    bool operator==(SpecialState const&) const = default;
};

struct TriggerState {
    struct Color {
        int channel;
        float fadeTime;
        ccColor3B color;
        float opacity;
        bool blending;
        bool playerColor1;
        bool playerColor2;
        int copyID;
        ccHSVValue copyHSV;
        bool copyOpacity;
        // for some reason default doesn't work for this one
        inline bool operator==(Color const& other) const {
            return channel == other.channel &&
                fadeTime == other.fadeTime &&
                color == other.color &&
                opacity == other.opacity &&
                blending == other.blending &&
                playerColor1 == other.playerColor1 &&
                playerColor2 == other.playerColor2 &&
                copyID == other.copyID &&
                copyHSV == other.copyHSV &&
                copyOpacity == other.copyOpacity;
        }
    };

    struct Move {
        float x;
        float y;
        bool lockToPlayerX;
        bool lockToPlayerY;
        EasingType easing;
        float easingRate;
        float time;
        int targetGroupID;
        bool useTarget;
        int followGroupID;
        MoveTargetType followType;
        bool operator==(Move const&) const = default;
    };

    struct Stop {
        int targetGroupID;
        bool operator==(Stop const&) const = default;
    };

    struct Pulse {
        int targetID;
        bool targetGroup;
        bool mainOnly;
        bool detailOnly;
        ccColor3B color;
        bool pulseHSV;
        int copyID;
        ccHSVValue copyHSV;
        float fadeIn;
        float hold;
        float fadeOut;
        bool exclusive;
        bool operator==(Pulse const&) const = default;
    };

    struct Alpha {
        int targetGroupID;
        float time;
        float opacity;
        bool operator==(Alpha const&) const = default;
    };

    struct Toggle {
        int targetGroupID;
        bool activateGroup;
        bool operator==(Toggle const&) const = default;
    };

    struct Spawn {
        int targetGroupID;
        float delayTime;
        bool editorDisable;
        bool operator==(Spawn const&) const = default;
    };

    struct Rotate {
        int targetGroupID;
        int centerGroupID;
        int degrees;
        int times360;
        bool lockObjectRotation;
        EasingType easing;
        float easingRate;
        float time;
        bool operator==(Rotate const&) const = default;
    };

    struct Follow {
        int targetGroupID;
        int followGroupID;
        float xMod;
        float yMod;
        float time;
        bool operator==(Follow const&) const = default;
    };

    struct Shake {
        float strength;
        float interval;
        float duration;
        bool operator==(Shake const&) const = default;
    };

    struct Animation {
        int targetGroupID;
        int animationID;
        bool operator==(Animation const&) const = default;
    };

    struct FollowPlayerY {
        int targetGroupID;
        float speed;
        float delay;
        int offset;
        float maxSpeed;
        float time;
        bool operator==(FollowPlayerY const&) const = default;
    };

    struct Touch {
        int targetGroupID;
        bool dualMode;
        bool holdMode;
        TouchToggleMode toggleMode;
        bool operator==(Touch const&) const = default;
    };

    struct Count {
        int itemID;
        int targetGroupID;
        int targetCount;
        bool activateGroup;
        bool multiActivate;
        bool operator==(Count const&) const = default;
    };

    struct InstantCount {
        int itemID;
        int targetGroupID;
        int targetCount;
        bool activateGroup;
        ComparisonType comparisonType;
        bool operator==(InstantCount const&) const = default;
    };

    struct Pickup {
        int itemID;
        int count;
        bool operator==(Pickup const&) const = default;
    };

    struct Collision {
        int blockAID;
        int blockBID;
        int targetID;
        bool activateGroup;
        bool triggerOnExit;
        bool operator==(Collision const&) const = default;
    };

    struct OnDeath {
        int targetGroupID;
        bool activateGroup;
        bool operator==(OnDeath const&) const = default;
    };

    std::variant<
        Color, Move, Stop, Pulse, Alpha, Toggle, Spawn, Rotate,
        Follow, Shake, Animation, FollowPlayerY,
        Touch, Count, InstantCount, Pickup, Collision, OnDeath
    > props;

    // shared
    bool touchTrigger;
    bool spawnTrigger;
    bool multiTrigger;

    static std::optional<TriggerState> from(EffectGameObject* obj);
    void to(EffectGameObject* obj) const;
    bool operator==(TriggerState const&) const = default;
};

struct LevelSettingsState {
    Speed speed;
    int mode;
    bool flipGravity;
    bool mini;
    bool dual;
    bool twoPlayer;
    float songOffset;
    bool fadeIn;
    bool fadeOut;
    int bg;
    int ground;
    int groundLine;
    int font;

    static LevelSettingsState from(LevelSettingsObject* obj);
    void to(LevelSettingsObject* obj) const;
    bool operator==(LevelSettingsState const& other) const = default;
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
std::string toDiffString(ObjColorState const& value);
std::string toDiffString(ObjState const& value);
std::string toDiffString(LevelSettingsState const& value);
std::string toDiffString(TriggerState const& value);
std::string toDiffString(SpecialState const& value);
std::string toDiffString(std::vector<short> const& value);

template <class... T, std::size_t... Is>
std::string toDiffStringImpl2(std::index_sequence<Is...>, T&&... value) {
    std::string ret = "";
    ((ret += (Is == 0 ? "" : ",") + toDiffString(value)), ...);
    return ret;
}

template <class... T>
std::string toDiffStringImpl(T&&... value) {
    return toDiffStringImpl2(std::index_sequence_for<T...> {}, std::forward<T>(value)...);
}

template <class A, class B, class... T>
std::string toDiffString(A&& a, B&& b, T&&... value) {
    return toDiffStringImpl(
        std::forward<A>(a), std::forward<B>(b), std::forward<T>(value)...
    );
}

template <class T>
std::string toDiffString(T const& value) {
    if constexpr (std::is_enum_v<T>) {
        return fmt::format("{}", static_cast<int>(value));
    }
    else {
        return fmt::format("{}", value);
    }
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
    virtual ~EditorEventData() = default;
};

struct ObjEventData : public EditorEventData {
    Ref<GameObject> obj;
    ObjEventData(Ref<GameObject> obj);

    virtual std::string getIconName() const = 0;
    virtual std::string getDescFmt() const = 0;
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

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjRemoved : public ObjEventData {
    inline ObjRemoved(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjPasted : public ObjEventData {
    Ref<GameObject> src;
    inline ObjPasted(Ref<GameObject> obj, Ref<GameObject> src)
      : ObjEventData(obj), src(src) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjTransformed : public ObjEventData {
    Transform<CCPoint> pos;
    Transform<float> angle;
    Transform<float> scale;
    Transform<bool> flipX;
    Transform<bool> flipY;

    inline ObjTransformed(
        Ref<GameObject> obj,
        Transform<CCPoint> const& pos,
        Transform<float> const& angle,
        Transform<float> const& scale,
        Transform<bool> const& flipX,
        Transform<bool> const& flipY
    ) : ObjEventData(obj),
        pos(pos), angle(angle), scale(scale),
        flipX(flipX), flipY(flipY) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjColored : public ObjEventData {
    Transform<ObjColorState> color;
    inline ObjColored(Ref<GameObject> obj, Transform<ObjColorState> const& color)
      : ObjEventData(obj), color(color) {}

    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
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

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjSelected : public ObjEventData {
    inline ObjSelected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct ObjDeselected : public ObjEventData {
    inline ObjDeselected(Ref<GameObject> obj)
      : ObjEventData(obj) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct StartPosChanged : public ObjEventData {
    Transform<LevelSettingsState> state;

    inline StartPosChanged(Ref<StartPosObject> obj, Transform<LevelSettingsState> const& state)
      : ObjEventData(obj.data()), state(state) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct TriggerPropsChanged : public ObjEventData {
    Transform<TriggerState> state;

    inline TriggerPropsChanged(Ref<EffectGameObject> obj, Transform<TriggerState> const& state)
      : ObjEventData(obj.data()), state(state) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
};

struct SpecialPropsChanged : public ObjEventData {
    Transform<SpecialState> state;

    inline SpecialPropsChanged(Ref<GameObject> obj, Transform<SpecialState> const& state)
      : ObjEventData(obj.data()), state(state) {}
    
    std::string toDiffString() const override;
    EditorEventData* clone() const override;
    void undo() const override;
    void redo() const override;
    std::vector<Detail> details() const override;

    std::string getIconName() const override;
    std::string getDescFmt() const override;
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

struct LevelSettingsChanged : public EditorEvent {
    Transform<LevelSettingsState> state;

    inline LevelSettingsChanged(Transform<LevelSettingsState> const& state)
      : state(state) {}
    
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
                if (res.size() > 6) break;
            }
            if (res.size() > 6) break;
        }
        return res;
    }

    CCNode* icon() const override {
        if (events.empty()) {
            return nullptr;
        }
        auto base = CCSprite::createWithSpriteFrameName("square_01_001.png");
        auto top = CCSprite::createWithSpriteFrameName(events.at(0).getIconName().c_str());
        top->setPosition({ base->getContentSize().width - 5.f, 5.f });
        base->addChild(top);
        base->setScale(.5f);
        return base;
    }

    std::string desc() const override {
        if (events.empty()) {
            return "Invalid Event";
        }
        auto& event = events.at(0);
        const char* ty;
        if (typeinfo_cast<StartPosObject*>(event.obj.data())) {
            ty = "Start Pos";
        }
        else if (typeinfo_cast<EffectGameObject*>(event.obj.data())) {
            ty = "Trigger";
        }
        else {
            ty = "Object";
        }
        return fmt::format(
            fmt::runtime(event.getDescFmt()),
            (events.size() == 1 ? ty : fmt::format("{} {}s", events.size(), ty))
        );
    }
};

struct EditorFilter : public EventFilter<EditorEvent> {
public:
    using Callback = void(EditorEvent*);

    ListenerResult handle(utils::MiniFunction<Callback> fn, EditorEvent* event);
    EditorFilter() = default;
};
