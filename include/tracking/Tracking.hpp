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

    struct BE_DLL ValueHolder {
        virtual ~ValueHolder() = default;
    };

    BE_DLL void addObjectKey(int key);
    BE_DLL void removeObjectKey(int key);

    class BE_DLL StateValue final {
    private:
        using Value = std::variant<
            bool,
            float,
            int,
            cocos2d::ccColor3B,
            cocos2d::ccHSVValue,
            cocos2d::CCPoint,
            std::vector<short>  // groups
        >;
        Value m_value;
        ObjectKey m_key;
    
    public:
        using List = std::vector<StateValue>;

        static StateValue from(GameObject* obj, ObjectKey key);
        template <class... Keys>
        static List list(GameObject* obj, Keys... keys) {
            List res;
            (res.push_back(StateValue::from(obj, keys)), ...);
            return res;
        }
        static List all(GameObject* obj);

        ObjectKey getKey() const;

        void assign(GameObject* obj) const;

        bool operator==(StateValue const& other) const;
        bool operator!=(StateValue const& other) const;
        bool isSameState(StateValue const& other) const;
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

    template <class P>
        requires requires(P* p) {
            { p->clone() } -> std::convertible_to<void*>;
        }
    class ClonedPtr final {
    private:
        P* m_ptr;
    
        ClonedPtr(P* ptr) : m_ptr(ptr) {}

    public:
        static ClonedPtr take(P* ptr) {
            return ClonedPtr(ptr);
        }
        static ClonedPtr clone(P* ptr) {
            return ClonedPtr(static_cast<P*>(ptr->clone()));
        }

        ClonedPtr(ClonedPtr const& other) : ClonedPtr(static_cast<P*>(other.get()->clone())) {}
        ~ClonedPtr() {
            delete m_ptr;
        }
        
        P* get() {
            return m_ptr;
        }
        P const* get() const {
            return m_ptr;
        }

        P* operator->() {
            return m_ptr;
        }
        P const* operator->() const {
            return m_ptr;
        }
    };

    class Collector {
    public:
        virtual bool push(EditorEvent* event) = 0;
    };

    template <class E = EditorEvent>
    class Collect;

    class BE_DLL EditorEvent : public geode::Event {
    protected:
        static std::vector<Collector*> s_groupCollection;

        template <class E>
        friend class Collect;

    public:
        void post();

        virtual std::string getName() const = 0;
        virtual EditorEvent* clone() const = 0;
        virtual void undo() const = 0;
        virtual void redo() const = 0;
    };

    class BE_DLL GroupedEditorEvent : public EditorEvent {
    protected:
        std::string m_name;
        std::vector<ClonedPtr<EditorEvent>> m_events;

    public:
        static GroupedEditorEvent from(
            std::vector<ClonedPtr<EditorEvent>> const& events,
            std::string const& name
        );
        std::string getName() const override;
        EditorEvent* clone() const override;
        void undo() const override;
        void redo() const override;
    };

    class BE_DLL ObjectsPlacedEvent : public EditorEvent {
    protected:
        std::vector<std::pair<geode::Ref<GameObject>, cocos2d::CCPoint>> m_objects;
    
    public:
        static ObjectsPlacedEvent from(std::vector<GameObject*> const& objs);
        std::string getName() const override;
        EditorEvent* clone() const override;
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
        EditorEvent* clone() const override;
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
        EditorEvent* clone() const override;
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
        EditorEvent* clone() const override;
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
        EditorEvent* clone() const override;
        void undo() const override;
        void redo() const override;
        void merge(ObjectsEditedEvent const& other);
    };

    template <class E>
    class Collect final : public Collector {
        std::string m_name;
        std::vector<ClonedPtr<E>> m_events;
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

        bool push(EditorEvent* event) override {
            if (auto e = geode::cast::typeinfo_cast<E*>(event)) {
                m_events.push_back(ClonedPtr<E>::clone(e));
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
                    std::is_same_v<E, ObjectsDeselectedEvent> ||
                    std::is_same_v<E, ObjectsEditedEvent>
                ) {
                    auto first = m_events.front();
                    for (auto i = m_events.begin() + 1; i != m_events.end(); i++) {
                        first->merge(*i->get());
                    }
                    first->post();
                }
                else {
                    GroupedEditorEvent::from(m_events, m_name).post();
                }
                m_events.clear();
            }
        }
    };

    struct BE_DLL EditorFilter : public geode::EventFilter<EditorEvent> {
    public:
        using Callback = void(EditorEvent*);

        geode::ListenerResult handle(geode::utils::MiniFunction<Callback> fn, EditorEvent* event);
        EditorFilter() = default;
    };
}
