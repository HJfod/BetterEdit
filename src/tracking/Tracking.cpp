#include "Tracking.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>

#define BLOCKED_CALL(...) \
    {\
        auto block = BlockAll();\
        __VA_ARGS__;\
    }

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
    return fmt::format("({},{})", point.x, point.y);
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
            if (events.size()) {
                post(MultiObjEvent(std::move(events)));
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
        if (BlockAll::blocked()) return;
        if (s_current) {
            s_current->events.emplace_back(std::forward<Args>(args)...);
        }
        else {
            post(MultiObjEvent<T>({ T(std::forward<Args>(args)...) }));
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
    return std::unique_ptr<EditorEvent>(static_cast<EditorEvent*>(this->clone()));
}

ObjEventData::ObjEventData(Ref<GameObject> obj)
  : obj(obj) {}

// ObjPlaced

std::string ObjPlaced::toDiffString() const {
    return fmtDiffString("add", obj->m_objectID, pos);
}

EditorEventData* ObjPlaced::clone() const {
    return new ObjPlaced(obj, pos);
}

void ObjPlaced::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
}

void ObjPlaced::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
    EditorUI::get()->moveObject(obj, pos - obj->getPosition());
    EditorUI::get()->selectObject(obj, true);
}

// ObjRemoved

std::string ObjRemoved::toDiffString() const {
    return fmtDiffString("rem", obj);
}

EditorEventData* ObjRemoved::clone() const {
    return new ObjRemoved(obj);
}

void ObjRemoved::undo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->addToSection(obj);
    LevelEditorLayer::get()->addSpecial(obj);
}

void ObjRemoved::redo() const {
    auto _ = BlockAll();
    LevelEditorLayer::get()->removeObjectFromSection(obj);
    LevelEditorLayer::get()->removeSpecial(obj);
    EditorUI::get()->deselectObject(obj);
}

// ObjMoved

std::string ObjMoved::toDiffString() const {
    return fmtDiffString("mov", obj, pos);
}

EditorEventData* ObjMoved::clone() const {
    return new ObjMoved(obj, pos);
}

void ObjMoved::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjMoved::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

// ObjRotated

std::string ObjRotated::toDiffString() const {
    return fmtDiffString("rot", obj, pos, angle);
}

EditorEventData* ObjRotated::clone() const {
    return new ObjRotated(obj, pos, angle);
}

void ObjRotated::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->rotateObjects(
        CCArray::createWithObject(obj), angle.from, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjRotated::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->rotateObjects(
        CCArray::createWithObject(obj), angle.to, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

// ObjScaled

std::string ObjScaled::toDiffString() const {
    return fmtDiffString("scl", obj, pos, scale);
}

EditorEventData* ObjScaled::clone() const {
    return new ObjScaled(obj, pos, scale);
}

void ObjScaled::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->scaleObjects(
        CCArray::createWithObject(obj), scale.from, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjScaled::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->scaleObjects(
        CCArray::createWithObject(obj), scale.to, obj->getPosition()
    );
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

// ObjFlipX

std::string ObjFlipX::toDiffString() const {
    return fmtDiffString("fpx", obj, pos, flip);
}

EditorEventData* ObjFlipX::clone() const {
    return new ObjFlipX(obj, pos, flip);
}

void ObjFlipX::undo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsX(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjFlipX::redo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsX(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

// ObjFlipY

std::string ObjFlipY::toDiffString() const {
    return fmtDiffString("fpy", obj, pos, flip);
}

EditorEventData* ObjFlipY::clone() const {
    return new ObjFlipY(obj, pos, flip);
}

void ObjFlipY::undo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsY(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.from - obj->getPosition());
}

void ObjFlipY::redo() const {
    auto _ = BlockAll();
    if (flip.from != flip.to) {
        EditorUI::get()->flipObjectsY(CCArray::createWithObject(obj));
    }
    EditorUI::get()->moveObject(obj, pos.to - obj->getPosition());
}

// ObjSelected

std::string ObjSelected::toDiffString() const {
    return fmtDiffString("sel", obj);
}

EditorEventData* ObjSelected::clone() const {
    return new ObjSelected(obj);
}

void ObjSelected::undo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
}

void ObjSelected::redo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

// ObjDeselected

std::string ObjDeselected::toDiffString() const {
    return fmtDiffString("dsl", obj);
}

EditorEventData* ObjDeselected::clone() const {
    return new ObjDeselected(obj);
}

void ObjDeselected::undo() const {
    auto _ = BlockAll();
    auto arr = EditorUI::get()->getSelectedObjects();
    arr->addObject(obj);
    EditorUI::get()->selectObjects(arr, false);
}

void ObjDeselected::redo() const {
    auto _ = BlockAll();
    EditorUI::get()->deselectObject(obj);
}

ListenerResult EditorFilter::handle(utils::MiniFunction<Callback> fn, EditorEvent* event) {
    fn(event);
    return ListenerResult::Propagate;
}

class $modify(LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        BLOCKED_CALL(LevelEditorLayer::addSpecial(obj));
        Bubble<ObjPlaced>::push(obj, obj->getPosition());
    }

    CCArray* createObjectsFromString(gd::string str, bool undo) {
        auto bubble = Bubble<ObjPlaced>();
        return LevelEditorLayer::createObjectsFromString(str, undo);
    }

    void createObjectsFromSetup(gd::string str) {
        auto bubble = Bubble<ObjPlaced>();
        LevelEditorLayer::createObjectsFromSetup(str);
        bubble.cancel();
    }

    void removeSpecial(GameObject* obj) {
        BLOCKED_CALL(LevelEditorLayer::removeSpecial(obj));
        Bubble<ObjRemoved>::push(obj);
    }
};

class $modify(EditorUI) {
    void moveObject(GameObject* obj, CCPoint to) {
        auto from = obj->getPosition();
        BLOCKED_CALL(EditorUI::moveObject(obj, to));
        Bubble<ObjMoved>::push(obj, Transform<CCPoint> { from, from + to });
    }

    void moveObjectCall(EditCommand command) {
        auto bubble = Bubble<ObjMoved>();
        EditorUI::moveObjectCall(command);
    }

    void deselectAll() {
        auto bubble = Bubble<ObjDeselected>();
        EditorUI::deselectAll();
    }

    void selectObjects(CCArray* objs, bool filter) {
        auto bubble = Bubble<ObjSelected>();
        EditorUI::selectObjects(objs, filter);
    }

    void scaleObjects(CCArray* objs, float scale, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> scales;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            scales.push_back({ obj, obj->getPosition(), obj->m_scale });
        }
        BLOCKED_CALL(EditorUI::scaleObjects(objs, scale, center));
        auto bubble = Bubble<ObjScaled>();
        for (auto& [obj, pos, from] : scales) {
            Bubble<ObjScaled>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<float> { from, obj->m_scale }
            );
        }
    }

    void rotateObjects(CCArray* objs, float angle, CCPoint center) {
        std::vector<std::tuple<GameObject*, CCPoint, float>> angles;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            angles.push_back({ obj, obj->getPosition(), obj->m_rotation });
        }
        BLOCKED_CALL(EditorUI::rotateObjects(objs, angle, center));
        auto bubble = Bubble<ObjRotated>();
        for (auto& [obj, pos, from] : angles) {
            Bubble<ObjRotated>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<float> { from, obj->m_rotation }
            );
        }
    }

    void flipObjectsX(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedX });
        }
        BLOCKED_CALL(EditorUI::flipObjectsX(objs));
        auto bubble = Bubble<ObjFlipX>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipX>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, obj->m_isFlippedX }
            );
        }
    }

    void flipObjectsY(CCArray* objs) {
        std::vector<std::tuple<GameObject*, CCPoint, bool>> flips;
        for (auto& obj : CCArrayExt<GameObject>(objs)) {
            flips.push_back({ obj, obj->getPosition(), obj->m_isFlippedY });
        }
        BLOCKED_CALL(EditorUI::flipObjectsY(objs));
        auto bubble = Bubble<ObjFlipY>();
        for (auto& [obj, pos, from] : flips) {
            Bubble<ObjFlipY>::push(
                obj,
                Transform<CCPoint> { pos, obj->getPosition() },
                Transform<bool> { from, obj->m_isFlippedY }
            );
        }
    }
};

class $modify(GameObject) {
    void selectObject(ccColor3B color) {
        GameObject::selectObject(color);
        Bubble<ObjSelected>::push(this);
    }

    void deselectObject() {
        GameObject::deselectObject();
        Bubble<ObjDeselected>::push(this);
    }
};
