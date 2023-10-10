#include <Utils.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/OBB2D.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

std::string editor_api::zLayerToString(ZLayer z) {
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

CCArrayExt<GameObject> editor_api::iterTargets(GameObject* target, CCArray* targets) {
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

CCArrayExt<GameObject> editor_api::iterSelected(EditorUI* ui) {
    return iterTargets(ui->m_selectedObject, ui->m_selectedObjects);
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

CCArray* editor_api::selectStructure(EditorUI* ui, GameObject* from) {
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
            default: break;
        }
    }
    auto arr = CCArray::create();
    arr->addObject(from);
    recursiveAddNear(ui, from, nearby, arr);
    return arr;
}

CCArray* editor_api::selectStructure(EditorUI* ui, CCArray* from) {
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

ButtonSprite* editor_api::createEditorButtonSprite(const char* top, const char* bg) {
    return ButtonSprite::create(
        CCSprite::createWithSpriteFrameName(top),
        32, true, 32, bg, 1.f
    );
}

void editor_api::moveGameLayerTo(EditorUI* ui, CCPoint const& pos, bool smooth, bool relative) {
    constexpr int GAME_LAYER_MOVE_ACTION_TAG = -48237;
    auto layer = ui->m_editorLayer->m_objectLayer;
    // Stop any running movements
    layer->stopActionByTag(GAME_LAYER_MOVE_ACTION_TAG);
    auto a = CCEaseInOut::create(
        CCMoveTo::create(
            (smooth ? .4f : .0f),
            (relative ? layer->getPosition() + pos : pos)
        ),
        2.f
    );
    a->setTag(GAME_LAYER_MOVE_ACTION_TAG);
    layer->runAction(a);
}

void editor_api::moveGameLayerTo(EditorUI* ui, GameObject* obj, bool smooth) {
    moveGameLayerTo(
        ui,
        (-obj->getPosition() + CCDirector::get()->getWinSize() / 2) *
            ui->m_editorLayer->m_objectLayer->getScale(),
        smooth,
        false
    );
}

void editor_api::moveGameLayerTo(EditorUI* ui, CCArray* objs, bool smooth) {
    constexpr float WINDOW_EDGE_SIZE = 120.f;

    auto layer = ui->m_editorLayer->m_objectLayer;
    auto pos = ui->getGroupCenter(objs, false);
    auto winSize = CCDirector::get()->getWinSize();

    auto gpos = layer->convertToWorldSpace(pos);
    auto npos = (-pos + winSize / 2) * layer->getScale();
    auto opos = layer->getPosition();

    // clamping so the position doesn't go past screen boundaries
    auto mx = gpos.x < WINDOW_EDGE_SIZE || gpos.x > winSize.width  - WINDOW_EDGE_SIZE;
    auto my = gpos.y < WINDOW_EDGE_SIZE || gpos.y > winSize.height - WINDOW_EDGE_SIZE;
    moveGameLayerTo(ui, { (mx ? npos.x : opos.x), (my ? npos.y : opos.y) }, smooth, false);
}
