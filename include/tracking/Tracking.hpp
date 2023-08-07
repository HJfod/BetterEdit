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
    enum class ObjectKey : int {
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
        EditorLayer1            = 20,
        EditorLayer2            = 61,
        GroupIDs                = 57,
    };

    BE_DLL void addObjectKey(int key);
    BE_DLL void removeObjectKey(int key);

    struct StateValue {
        using Value = std::variant<
            bool,
            float,
            int,
            cocos2d::ccColor3B,
            cocos2d::ccHSVValue,
            cocos2d::CCPoint,
            std::vector<short>  // groups
        >;
        Value value;
        ObjectKey key;

        using List = std::vector<StateValue>;

        static StateValue from(GameObject* obj, ObjectKey key);

        template <class... Keys>
        static List list(GameObject* obj, Keys... keys) {
            List res;
            (res.push_back(StateValue::from(obj, keys)), ...);
            return res;
        }

        static List all(GameObject* obj);

        void assign(GameObject* obj) const;

        bool operator==(StateValue const& other) const;
        bool operator!=(StateValue const& other) const;
    };

    struct BE_DLL ChangedState {
        StateValue from, to;
        ChangedState(StateValue const& from, StateValue const& to);
    };

    class ObjectKeyMap final {
        geode::Ref<GameObject> m_object;
        std::vector<ChangedState> m_changes;
    
    public:
        static ObjectKeyMap from(GameObject* obj, StateValue::List const& previous);

        bool contains(ObjectKey key) const;
        void insert(ObjectKey key, ChangedState const& state);
        size_t size() const;
    };
    
    class EditorEvent;
    class GroupedEditorEvent;
    class ObjectsPlacedEvent;
    class ObjectsRemovedEvent;
    class ObjectsSelectedEvent;
    class ObjectsEditedEvent;

    class Collector {
    public:
        virtual bool push(EditorEvent const& event) = 0;
    };

    template <class E = EditorEvent>
    class Collect final : public Collector {
        std::string m_name;
        std::vector<std::unique_ptr<E>> m_events;
        bool m_dropped = false;
    
        void stop() {
            if (!m_dropped) {
                EditorEvent::s_groupCollection.pop_back();
                m_dropped = true;
            }
        }

    public:
        Collect(std::string const& name = "") : m_name(name) {
            EditorEvent::s_groupCollection.push_back(this);
        }
        ~Collect() {
            this->stop();
        }
        Collect(Collect const&) = delete;
        Collect(Collect&&) = delete;

        bool push(EditorEvent const& event) override {
            if (auto e = (geode::casts::typeinfo_cast<E*>(event))) {
                m_events.push_back(std::make_unique<E>(e));
                return true;
            }
            return false;
        }

        void post() {
            this->stop();
            if (m_events.size()) {
                // reduce object events to one
                if constexpr (
                    std::is_same_v<E, ObjectsPlacedEvent> ||
                    std::is_same_v<E, ObjectsRemovedEvent> ||
                    std::is_same_v<E, ObjectsSelectedEvent> ||
                    std::is_same_v<E, ObjectsEditedEvent>
                ) {
                    auto first = m_events.front();
                    for (auto i = m_events.begin() + 1; i != m_events.end(); i++) {
                        first->merge(**i);
                    }
                    first->post();
                }
                else {
                    GroupedEditorEvent(std::move(m_events), m_name).post();
                }
                m_events.clear();
            }
        }
    };

    class BE_DLL EditorEvent : public geode::Event {
    protected:
        static std::vector<Collector*> s_groupCollection;

        template <class E>
        friend class Collect;

    public:
        void post();

        virtual std::string getName() const = 0;
        virtual std::unique_ptr<EditorEvent> clone() const = 0;
        virtual void undo() const = 0;
        virtual void redo() const = 0;
    };

    class BE_DLL GroupedEditorEvent : public EditorEvent {
    protected:
        std::string m_name;
        std::vector<std::unique_ptr<EditorEvent>> m_events;

    public:
        static GroupedEditorEvent from(
            std::vector<std::unique_ptr<EditorEvent>>&& events,
            std::string const& name
        );
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
    };

    class BE_DLL ObjectsPlacedEvent : public EditorEvent {
    protected:
        std::vector<std::pair<geode::Ref<GameObject>, cocos2d::CCPoint>> m_objects;
    
    public:
        static ObjectsPlacedEvent from(std::vector<GameObject*> const& objs);
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsPlacedEvent const& other);
    };

    class BE_DLL ObjectsRemovedEvent : public EditorEvent {
    protected:
        std::vector<geode::Ref<GameObject>> m_objects;
    
    public:
        static ObjectsRemovedEvent from(std::vector<GameObject*> const& objs);
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsRemovedEvent const& other);
    };

    class BE_DLL ObjectsSelectedEvent : public EditorEvent {
    protected:
        std::vector<geode::Ref<GameObject>> m_objects;
    
    public:
        static ObjectsSelectedEvent from(std::vector<GameObject*> const& objs);
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsSelectedEvent const& other);
    };

    class BE_DLL ObjectsDeselectedEvent : public EditorEvent {
    protected:
        std::vector<geode::Ref<GameObject>> m_objects;
    
    public:
        static ObjectsDeselectedEvent from(std::vector<GameObject*> const& objs);
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsDeselectedEvent const& other);
    };

    class BE_DLL ObjectsEditedEvent : public EditorEvent {
    protected:
        std::vector<ObjectKeyMap> m_objects;
    
    public:
        static ObjectsEditedEvent from(std::vector<ObjectKeyMap> const& objs);
        std::string getName() const override;
        std::unique_ptr<EditorEvent> clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsEditedEvent const& other);
    };

    struct BE_DLL EditorFilter : public geode::EventFilter<EditorEvent> {
    public:
        using Callback = void(EditorEvent*);

        geode::ListenerResult handle(geode::utils::MiniFunction<Callback> fn, EditorEvent* event);
        EditorFilter() = default;
    };
}
