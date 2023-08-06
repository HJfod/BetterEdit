#pragma once

#include "../Macros.hpp"
#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/StartPosObject.hpp>

namespace better_edit {
    // based on https://github.com/Wyliemaster/gddocs/blob/master/docs/resources/client/level-components/level-object.md
    enum class ObjectKey {
        ID                      = 1,
        X                       = 2,
        Y                       = 3, 
        FlipX                   = 4,
        FlipY                   = 5,
        Angle                   = 6,
        ColorTriggerRed         = 7,
        ColorTriggerGreen       = 8,
        ColorTriggerBlue        = 9,
        TriggerDuration         = 10,
        TriggerTouchTrigger     = 11,
        // SecretCoinID            = 12,
        Checked                 = 13,
        BGColorTriggerTintGround= 14,
        ColorTriggerP1          = 15,
        ColorTriggerP2          = 16,
        ColorTriggerBlending    = 17,
        ColorChannelIDLegacy    = 19,
    };

    template <class A, class... T>
    concept IsOneOf = (std::is_same_v<A, T> || ...);

    struct StateValue {
        using Value = std::variant<
            bool,
            float,
            int,
            cocos2d::ccColor3B,
            cocos2d::ccHSVValue,
            ZLayer,
            EasingType,
            MoveTargetType,
            TouchToggleMode,
            ComparisonType,
            Speed,
            cocos2d::CCPoint,
            std::vector<short>,  // groups
        >;
        Value value;

        static StateValue from(GameObject* obj, ObjectKey key);
    };

    struct BE_DLL ChangedState {
        StateValue from, to;
        ChangedState(StateValue const& from, StateValue const& to);
    };

    struct BE_DLL BlockEventsHandle final {
        BlockEventsHandle();
        ~BlockEventsHandle();
        BlockEventsHandle(BlockEventsHandle const&) = delete;
        BlockEventsHandle(BlockEventsHandle&&) = delete;
    };

    // todo: use a vector for smaller memory footprint
    using ObjectKeyMap = std::unordered_map<ObjectKey, ChangedState>;

    class BE_DLL EditorEvent : public geode::Event {
    public:
        using ObjectPlaced = std::pair<geode::Ref<GameObject>, cocos2d::CCPoint>;
        using ObjectRemoved = geode::Ref<GameObject>;
        using ObjectSelected = std::pair<geode::Ref<GameObject>, bool>;
        using ObjectEdited = std::pair<geode::Ref<GameObject>, ObjectKeyMap>;

        using Type = std::variant<
            std::vector<ObjectPlaced>,
            std::vector<ObjectRemoved>,
            std::vector<ObjectSelected>,
            std::vector<ObjectEdited>,
        >;

    protected:
        Type m_event;
        static bool s_blockEvents;

        EditorEvent(Type const& type);
    
        friend struct BlockEventsHandle;

    public:
        static void post(Type const& type);
        template <class T>
            requires requires(T const& v) {
                Type(std::vector<T> { v });
            }
        static void post(T const& event) {
            return EditorEvent::post({  event });
        }
        static BlockEventsHandle block();
    };

    struct BE_DLL EditorFilter : public geode::EventFilter<EditorEvent> {
    public:
        using Callback = void(EditorEvent*);

        geode::ListenerResult handle(geode::utils::MiniFunction<Callback> fn, EditorEvent* event);
        EditorFilter() = default;
    };
}
