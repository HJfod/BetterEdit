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
        if (EditorUI::get() && !BlockAll::blocked()) {
            t.post();
        }
    }
};

std::unique_ptr<EditorEvent> EditorEvent::unique() const {
    return std::unique_ptr<EditorEvent>(this->clone());
}

ObjectEvent::ObjectEvent(Ref<GameObject> obj)
  : obj(obj) {}

ObjectPlacedEvent::ObjectPlacedEvent(Ref<GameObject> obj, CCPoint const& pos)
  : ObjectEvent(obj), pos(pos) {}

std::string ObjectPlacedEvent::toDiffString() const {
    return fmt::format("add {}, {}, {}", obj->m_objectID, pos.x, pos.y);
}

EditorEvent* ObjectPlacedEvent::clone() const {
    return new ObjectPlacedEvent(obj, pos);
}

void ObjectPlacedEvent::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
}

void ObjectPlacedEvent::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
    EditorUI::get()->moveObject(obj, pos);
}

ObjectRemovedEvent::ObjectRemovedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectRemovedEvent::toDiffString() const {
    return fmt::format("rem {}", ::toDiffString(obj));
}

EditorEvent* ObjectRemovedEvent::clone() const {
    return new ObjectRemovedEvent(obj);
}

void ObjectRemovedEvent::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
}

void ObjectRemovedEvent::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
}

ObjectTransformedEvent::ObjectTransformedEvent(
    Ref<GameObject> obj,
    Transform<CCPoint> const& pos,
    OptTransform<float> const& scale,
    OptTransform<float> const& angle,
    OptTransform<bool> const& flipX,
    OptTransform<bool> const& flipY
) : ObjectEvent(obj),
    pos(pos),
    scale(scale),
    angle(angle),
    flipX(flipX),
    flipY(flipY) {}

ObjectTransformedEvent ObjectTransformedEvent::moved(
    Ref<GameObject> obj, Transform<CCPoint> const& pos
) {
    return ObjectTransformedEvent(obj, pos, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
}

ObjectTransformedEvent ObjectTransformedEvent::scaled(
    Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> const& scale
) {
    return ObjectTransformedEvent(obj, pos, scale, std::nullopt, std::nullopt, std::nullopt);
}

ObjectTransformedEvent ObjectTransformedEvent::rotated(
    Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<float> const& angle
) {
    return ObjectTransformedEvent(obj, pos, std::nullopt, angle, std::nullopt, std::nullopt);
}

ObjectTransformedEvent ObjectTransformedEvent::flippedX(
    Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip
) {
    return ObjectTransformedEvent(obj, pos, std::nullopt, std::nullopt, flip, std::nullopt);
}

ObjectTransformedEvent ObjectTransformedEvent::flippedY(
    Ref<GameObject> obj, Transform<CCPoint> const& pos, Transform<bool> const& flip
) {
    return ObjectTransformedEvent(obj, pos, std::nullopt, std::nullopt, std::nullopt, flip);
}

std::string ObjectTransformedEvent::toDiffString() const {
    return fmt::format(
        "trf {}, {}, {}, {}, {}, {}",
        ::toDiffString(obj),
        ::toDiffString(pos),
        ::toDiffString(scale),
        ::toDiffString(angle),
        ::toDiffString(flipX),
        ::toDiffString(flipY)
    );
}

EditorEvent* ObjectTransformedEvent::clone() const {
    return new ObjectTransformedEvent(obj, pos, scale, angle, flipX, flipY);
}

void ObjectTransformedEvent::undo() const {
}

void ObjectTransformedEvent::redo() const {
}

ObjectSelectedEvent::ObjectSelectedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectSelectedEvent::toDiffString() const {
    return fmt::format("sel {}", ::toDiffString(obj));
}

EditorEvent* ObjectSelectedEvent::clone() const {
    return new ObjectSelectedEvent(obj);
}

void ObjectSelectedEvent::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
}

void ObjectSelectedEvent::redo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

ObjectDeselectedEvent::ObjectDeselectedEvent(Ref<GameObject> obj)
  : ObjectEvent(obj) {}

std::string ObjectDeselectedEvent::toDiffString() const {
    return fmt::format("dsl {}", ::toDiffString(obj));
}

EditorEvent* ObjectDeselectedEvent::clone() const {
    return new ObjectDeselectedEvent(obj);
}

void ObjectDeselectedEvent::undo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

void ObjectDeselectedEvent::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
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
        Bubble<ObjectTransformedEvent>::push(ObjectTransformedEvent::moved(
            obj, { from, to }
        ));
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjectTransformedEvent>();
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
        auto bubble = Bubble<ObjectTransformedEvent>();
        std::vector<std::tuple<GameObject*, CCPoint, float>> scales;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            scales.push_back({ obj, obj->getPosition(), obj->m_scale });
        }
        {
            auto _ = BlockAll();
            EditorUI::scaleObjects(objs, scale, center);
        }
        for (auto& [obj, pos, from] : scales) {
            Bubble<ObjectTransformedEvent>::push(ObjectTransformedEvent::scaled(
                obj, { pos, obj->getPosition() }, { from, obj->m_scale }
            ));
        }
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> angles;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            angles.push_back({ obj, obj->getPosition(), obj->m_rotation });
        }
        {
            auto _ = BlockAll();
            EditorUI::rotateObjects(objs, angle, center);
        }
        auto bubble = Bubble<ObjectTransformedEvent>();
        for (auto& [obj, pos, from] : angles) {
            Bubble<ObjectTransformedEvent>::push(ObjectTransformedEvent::rotated(
                obj, { pos, obj->getPosition() }, { from, obj->m_rotation }
            ));
        }
    }

    void flipObjectsX(CCArray* objs) {
        auto bubble = Bubble<ObjectTransformedEvent>();
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedX });
        }
        EditorUI::flipObjectsX(objs);
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjectTransformedEvent>::push(ObjectTransformedEvent::flippedX(
                obj, { pos, obj->getPosition() }, { from, obj->m_isFlippedX }
            ));
        }
    }

    void flipObjectsY(CCArray* objs) {
        auto bubble = Bubble<ObjectTransformedEvent>();
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedY });
        }
        EditorUI::flipObjectsY(objs);
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjectTransformedEvent>::push(ObjectTransformedEvent::flippedY(
                obj, { pos, obj->getPosition() }, { from, obj->m_isFlippedY }
            ));
        }
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
