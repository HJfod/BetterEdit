#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>

std::string toDiffString(Ref<GameObject> obj) {
    return fmt::format(
        "{}_{}_{}_{}_{}_{}_{}",
        obj->m_objectID,
        obj->getPosition().x,
        obj->getPosition().y,
        obj->getRotation(),
        obj->getScale(),
        obj->m_baseColorID,
        obj->m_detailColorID
    );
}

std::string toDiffString(CCPoint const& point) {
    return fmt::format("({}, {})", point.x, point.y);
}

std::string toDiffString(float value) {
    return fmt::format("{}", value);
}

std::string toDiffString(bool value) {
    return value ? "t" : "f";
}

struct BlockAll {
    static inline BlockAll* s_current = nullptr;
    static bool blocked() {
        return s_current;
    }
    BlockAll() {
        if (!s_current) {
            s_current = this;
        }
    }
    ~BlockAll() {
        if (s_current == this) {
            s_current = nullptr;
        }
    }
};

template <class T>
struct Bubble {
    T event;
    static inline Bubble<T>* s_current = nullptr;
    template <class... Args>
    Bubble(Args&&... args) : event({}, std::forward<Args>(args)...) {
        if (!s_current) {
            s_current = this;
        }
    }
    ~Bubble() {
        if (s_current == this) {
            s_current = nullptr;
            post(std::move(event));
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
            s_current->event.objs.emplace_back(std::forward<Args>(args)...);
        }
        else {
            post(T({{ std::forward<Args>(args)... }}));
        }
    }
private:
    template <class T>
    static void post(T&& t) {
        if (EditorUI::get() && !BlockAll::blocked()) {
            t.post();
        }
    }
};

std::unique_ptr<EditorEvent> EditorEvent::unique() const {
    return std::unique_ptr<EditorEvent>(this->clone());
}

ObjectPlacedEvent::ObjectPlacedEvent(ObjRefTuples<CCPoint> const& objs)
  : objs(objs) {}

std::string ObjectPlacedEvent::toDiffString() const {
    return fmt::format("add {}", ::toDiffString(objs));
}

EditorEvent* ObjectPlacedEvent::clone() const {
    return new ObjectPlacedEvent(objs);
}

void ObjectPlacedEvent::undo() const {
    auto _ = BlockAll();
    for (auto& [obj, _] : objs) {
        LevelEditorLayer::get()->removeObjectFromSection(obj);
        LevelEditorLayer::get()->removeSpecial(obj);
        EditorUI::get()->deselectObject(obj);
    }
}

void ObjectPlacedEvent::redo() const {
    auto _ = BlockAll();
    for (auto& [obj, pos] : objs) {
        LevelEditorLayer::get()->addToSection(obj);
        LevelEditorLayer::get()->addSpecial(obj);
        EditorUI::get()->moveObject(obj, pos);
    }
}

ObjectRemovedEvent::ObjectRemovedEvent(ObjRefs const& objs)
  : objs(objs) {}

std::string ObjectRemovedEvent::toDiffString() const {
    return fmt::format("rem {}", ::toDiffString(objs));
}

EditorEvent* ObjectRemovedEvent::clone() const {
    return new ObjectRemovedEvent(objs);
}

void ObjectRemovedEvent::undo() const {
    auto _ = BlockAll();
    for (auto& obj : objs) {
        LevelEditorLayer::get()->addToSection(obj);
        LevelEditorLayer::get()->addSpecial(obj);
    }
}

void ObjectRemovedEvent::redo() const {
    auto _ = BlockAll();
    for (auto& obj : objs) {
        LevelEditorLayer::get()->removeObjectFromSection(obj);
        LevelEditorLayer::get()->removeSpecial(obj);
        EditorUI::get()->deselectObject(obj);
    }
}

ObjectMovedEvent::ObjectMovedEvent(ObjRefTuples<CCPoint, CCPoint> const& objs)
  : objs(objs) {}

std::string ObjectMovedEvent::toDiffString() const {
    return fmt::format("mov {}", ::toDiffString(objs));
}

EditorEvent* ObjectMovedEvent::clone() const {
    return new ObjectMovedEvent(objs);
}

void ObjectMovedEvent::undo() const {
    auto _ = BlockAll();
    for (auto& [obj, from, _] : objs) {
        EditorUI::get()->moveObject(obj, from);
    }
}

void ObjectMovedEvent::redo() const {
    auto _ = BlockAll();
    for (auto& [obj, _, to] : objs) {
        EditorUI::get()->moveObject(obj, to);
    }
}

ObjectRotatedEvent::ObjectRotatedEvent(ObjRefs const& objs, float from, float to)
  : objs(objs), from(from), to(to) {}

std::string ObjectRotatedEvent::toDiffString() const {
    return fmt::format("rot {}, {}, {}", ::toDiffString(objs), from, to);
}

EditorEvent* ObjectRotatedEvent::clone() const {
    return new ObjectRotatedEvent(objs, from, to);
}

ObjectScaledEvent::ObjectScaledEvent(ObjRefs const& objs, float from, float to)
  : objs(objs), from(from), to(to) {}

std::string ObjectScaledEvent::toDiffString() const {
    return fmt::format("scl {}, {}, {}", ::toDiffString(objs), from, to);
}

EditorEvent* ObjectScaledEvent::clone() const {
    return new ObjectScaledEvent(objs, from, to);
}

ObjectFlippedXEvent::ObjectFlippedXEvent(ObjRefTuples<bool> const& objs)
  : objs(objs) {}

std::string ObjectFlippedXEvent::toDiffString() const {
    return fmt::format("flx {}", ::toDiffString(objs));
}

EditorEvent* ObjectFlippedXEvent::clone() const {
    return new ObjectFlippedXEvent(objs);
}

ObjectFlippedYEvent::ObjectFlippedYEvent(ObjRefTuples<bool> const& objs)
  : objs(objs) {}

std::string ObjectFlippedYEvent::toDiffString() const {
    return fmt::format("fly {}", ::toDiffString(objs));
}

EditorEvent* ObjectFlippedYEvent::clone() const {
    return new ObjectFlippedYEvent(objs);
}

ObjectSelectedEvent::ObjectSelectedEvent(ObjRefs const& objs)
  : objs(objs) {}

std::string ObjectSelectedEvent::toDiffString() const {
    return fmt::format("sel {}", ::toDiffString(objs));
}

EditorEvent* ObjectSelectedEvent::clone() const {
    return new ObjectSelectedEvent(objs);
}

ObjectDeselectedEvent::ObjectDeselectedEvent(ObjRefs const& objs)
  : objs(objs) {}

std::string ObjectDeselectedEvent::toDiffString() const {
    return fmt::format("dsl {}", ::toDiffString(objs));
}

EditorEvent* ObjectDeselectedEvent::clone() const {
    return new ObjectDeselectedEvent(objs);
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}

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
        auto from = obj->getPosition();
        EditorUI::moveObject(obj, to);
        Bubble<ObjectMovedEvent>::push(obj, from, to);
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

    void scaleObjects(CCArray* objs, float scale, CCPoint center) {
        auto bubble = Bubble<ObjectScaledEvent>();
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            Bubble<ObjectScaledEvent>::push(obj, scale);
        }
        EditorUI::scaleObjects(objs, scale, center);
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        auto bubble = Bubble<ObjectRotatedEvent>();
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            Bubble<ObjectRotatedEvent>::push(obj, angle);
        }
        EditorUI::rotateObjects(objs, angle, center);
    }

    void flipObjectsX(CCArray* objs) {
        auto bubble = Bubble<ObjectFlippedXEvent>();
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            Bubble<ObjectFlippedXEvent>::push(obj, !obj->isFlipX());
        }
        EditorUI::flipObjectsX(objs);
    }

    void flipObjectsY(CCArray* objs) {
        auto bubble = Bubble<ObjectFlippedYEvent>();
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            Bubble<ObjectFlippedYEvent>::push(obj, !obj->isFlipY());
        }
        EditorUI::flipObjectsY(objs);
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
