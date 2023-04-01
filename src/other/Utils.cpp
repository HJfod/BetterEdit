#include "Utils.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/OBB2D.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <clipper2/clipper.h>
#include <tracking/Tracking.hpp>

std::string zLayerToString(ZLayer z) {
    switch (z) {
        case ZLayer::B4:      return "B4";
        case ZLayer::B3:      return "B3";
        case ZLayer::B2:      return "B2";
        case ZLayer::B1:      return "B1";
        case ZLayer::Default: return "Default";
        case ZLayer::T1:      return "T1";
        case ZLayer::T2:      return "T2";
        case ZLayer::T3:      return "T3";
    }
    return "Unknown";
}

CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets) {
    if (target) {
        return CCArrayExt<GameObject>(CCArray::createWithObject(target));
    }
    else {
        if (targets->containsObject(nullptr)) {
            auto arr = CCArray::create();
            for (auto obj : CCArrayExt<CCObject>(targets)) {
                if (obj) {
                    arr->addObject(obj);
                }
            }
            return arr;
        }
        return targets;
    }
}

CCArrayExt<GameObject> iterSelected(EditorUI* ui) {
    return iterTargets(ui->m_selectedObject, ui->m_selectedObjects);
}

bool polygonIntersect(std::vector<CCPoint> const& a, CCPoint const& b) {
    using namespace Clipper2Lib;
    PointD pb { b.x, b.y };
    PathD path;
    for (auto& pt : a) {
        path.push_back({ pt.x, pt.y });
    }
    return PointInPolygon(pb, { path }) != PointInPolygonResult::IsOutside;
}

static bool objectsAreAdjacent(GameObject* o1, GameObject* o2) {
    auto rect1 = o1->boundingBox();
    auto rect2 = o2->boundingBox();
    return (
        rect1.origin.x <= rect2.origin.x + rect2.size.width &&
        rect2.origin.x <= rect1.origin.x + rect1.size.width &&
        rect1.origin.y <= rect2.origin.y + rect2.size.height &&
        rect2.origin.y <= rect1.origin.y + rect1.size.height
    );
}

static void recursiveAddNear(EditorUI* ui, GameObject* fromObj, std::vector<GameObject*> const& vnear, CCArray* arr) {
    for (auto const& obj : vnear) {
        if (objectsAreAdjacent(fromObj, obj)) {
            if (!arr->containsObject(obj)) {
                arr->addObject(obj);
                recursiveAddNear(ui, obj, vnear, arr);
            }
        }
    }
}

CCArray* selectStructure(EditorUI* ui, GameObject* from) {
    if (!from) return CCArray::create();
    std::vector<GameObject*> nearby;
    auto pos = from->getPosition();
    for (auto obj : CCArrayExt<GameObject>(ui->m_editorLayer->m_objects)) {
        switch (obj->m_objectType) {
            case GameObjectType::Slope:
            case GameObjectType::Solid:
            case GameObjectType::Special:
            case GameObjectType::Decoration:
            case GameObjectType::Hazard:
            case GameObjectType::GravityPad:
            case GameObjectType::PinkJumpPad:
            case GameObjectType::YellowJumpPad:
            case GameObjectType::RedJumpPad: {
                if (obj->m_editorLayer == from->m_editorLayer) {
                    if (ccpDistance(obj->getPosition(), pos) < 500.0f) {
                        nearby.push_back(obj);
                    }
                }
            } break;
        }
    }
    auto arr = CCArray::create();
    arr->addObject(from);
    recursiveAddNear(ui, from, nearby, arr);
    return arr;
}

CCArray* selectStructure(EditorUI* ui, CCArray* from) {
    CCArray* res = CCArray::create();
    for (auto fr : CCArrayExt<GameObject>(from)) {
        for (auto obj : CCArrayExt<GameObject>(selectStructure(ui, fr))) {
            if (!res->containsObject(obj)) {
                res->addObject(obj);
            }
        }
    }
    return res;
}

class $modify(TintLayer, LevelEditorLayer) {
    std::unordered_set<Ref<GameObject>> tinted {};

    void updateVisibility(float dt);
};

class $modify(TintGameObject, GameObject) {
    std::optional<ccColor3B> tint;

    void updateTintColor() {
        auto lel = static_cast<TintLayer*>(LevelEditorLayer::get());
        if (m_fields->tint) {
            this->setObjectColor(m_fields->tint.value());
            if (m_detailSprite) {
                m_detailSprite->setColor(m_fields->tint.value());
                m_detailSprite->setChildColor(m_fields->tint.value());
            }
            lel->m_fields->tinted.insert(this);
        }
        else {
            lel->m_fields->tinted.erase(this);
        }
    }
};

void TintLayer::updateVisibility(float dt) {
    LevelEditorLayer::updateVisibility(dt);
    if (m_fields->tinted.size()) {
        auto _ = BlockAll();
        for (auto obj : m_fields->tinted) {
            static_cast<TintGameObject*>(obj.data())->updateTintColor();
        }
    }
}

void tintObject(GameObject* obj, std::optional<ccColor3B> const& color) {
    auto _ = BlockAll();
    auto tobj = static_cast<TintGameObject*>(obj);
    tobj->m_fields->tint = color;
    tobj->updateTintColor();
}

ButtonSprite* createEditorButtonSprite(const char* top, const char* bg) {
    return ButtonSprite::create(
        CCSprite::createWithSpriteFrameName(top),
        32, true, 32, bg, 1.f
    );
}
