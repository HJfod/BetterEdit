#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>

ObjectEvent::ObjectEvent(Ref<GameObject> obj)
  : obj(obj) {}

ObjectPlacedEvent::ObjectPlacedEvent(Ref<GameObject> obj, CCPoint const& pos)
  : ObjectEvent(obj), pos(pos) {}

std::string ObjectPlacedEvent::toDiffString() const {
    return fmt::format("+ {} at {}, {}", obj->m_objectID, pos.x, pos.y);
}

ObjectRemovedEvent::ObjectRemovedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectRemovedEvent::toDiffString() const {
    return fmt::format("- {}", obj->m_uniqueID);
}

ObjectMovedEvent::ObjectMovedEvent(Ref<GameObject> obj, CCPoint const& pos)
  : ObjectEvent(obj), pos(pos) {}

std::string ObjectMovedEvent::toDiffString() const {
    return fmt::format("> {} to {}, {}", obj->m_uniqueID, pos.x, pos.y);
}

ObjectSelectedEvent::ObjectSelectedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectSelectedEvent::toDiffString() const {
    return fmt::format("@ {}", obj->m_uniqueID);
}

ObjectDeselectedEvent::ObjectDeselectedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectDeselectedEvent::toDiffString() const {
    return fmt::format("~ {}", obj->m_uniqueID);
}

EditorEvent::EditorEvent(Kind const& kind) : kind(kind) {}

std::string EditorEvent::toDiffString() const {
    return std::visit([](auto const& ev) {
        return ev.toDiffString();
    }, kind);
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}

template <class T>
struct Bubble {
    std::vector<T> events;
    static inline Bubble<T>* s_current = nullptr;
    Bubble() {
        if (!s_current) {
            s_current = this;
        }
    }
    ~Bubble() {
        if (s_current == this) {
            s_current = nullptr;
            if (events.size() > 1) {
                post(MultiObjectEvent(std::move(events)));
            }
            else if (events.size() == 1) {
                post(events.front());
            }
        }
    }
    void cancel() {
        if (s_current == this) {
            s_current = nullptr;
        }
    }
    template <class... Args>
    static void push(Args&&... args) {
        if (s_current) {
            s_current->events.emplace_back(std::forward<Args>(args)...);
        }
        else {
            post(T(std::forward<Args>(args)...));
        }
    }
private:
    template <class T>
    static void post(T&& t) {
        if (EditorUI::get()) {
            EditorEvent(t).post();
        }
    }
};

class $modify(LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        Bubble<ObjectPlacedEvent>::push(obj, obj->getPosition());
    }

    CCArray* createObjectsFromString(gd::string str, bool undo) {
        auto bubble = Bubble<ObjectPlacedEvent>();
        auto objs = LevelEditorLayer::createObjectsFromString(str, undo);
        return objs;
    }

    void createObjectsFromSetup(gd::string str) {
        auto bubble = Bubble<ObjectPlacedEvent>();
        LevelEditorLayer::createObjectsFromSetup(str);
        bubble.cancel();
    }

    void removeSpecial(GameObject* obj) {
        LevelEditorLayer::removeSpecial(obj);
        Bubble<ObjectRemovedEvent>::push(obj);
    }
};

class $modify(EditorUI) {
    void moveObject(GameObject* obj, CCPoint to) {
        EditorUI::moveObject(obj, to);
        Bubble<ObjectMovedEvent>::push(obj, to);
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjectMovedEvent>();
        EditorUI::moveObjectCall(command);
    }

    void deselectAll() {
        auto bubble = Bubble<ObjectDeselectedEvent>();
        EditorUI::deselectAll();
    }

    void selectObjects(CCArray* objs, bool filter) {
        auto bubble = Bubble<ObjectSelectedEvent>();
        EditorUI::selectObjects(objs, filter);
    }
};

class $modify(GameObject) {
    void selectObject(ccColor3B color) {
        GameObject::selectObject(color);
        Bubble<ObjectSelectedEvent>::push(this);
    }

    void deselectObject() {
        GameObject::deselectObject();
        Bubble<ObjectDeselectedEvent>::push(this);
    }
};
