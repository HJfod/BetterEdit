#include "AutoLinkManager.hpp"
#include "AutoLinkObject.hpp"
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>

using namespace geode::prelude;

bool AutoLinkSet::init(Definitions const& defs) {
    m_definitions = defs;
    return true;
}

std::string AutoLinkSet::getObject(AutoLinkDefinition def) {
    if (m_definitions.count(def) && m_definitions.at(def).size()) {
        return m_definitions.at(def).front();
    }
    def.removeCorners();
    if (m_definitions.count(def) && m_definitions.at(def).size()) {
        return m_definitions.at(def).front();
    }
    if (m_definitions.size() && m_definitions.begin()->second.size()) {
        return m_definitions.begin()->second.front();
    }
    return "";
}

AutoLinkSet* AutoLinkSet::create(Definitions const& defs) {
    auto ret = new AutoLinkSet;
    if (ret && ret->init(defs)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::vector<Ref<AutoLinkSet>> AutoLinkManager::getSets() const {
    return m_sets;
}

namespace std {
    template <>
    struct std::hash<CCPoint> {
        std::size_t operator()(CCPoint const& pt) const noexcept {
            return std::hash<float>()(pt.x) ^ std::hash<float>()(pt.y);
        }
    };

    template <>
    struct equal_to<CCPoint> {
        bool operator()(CCPoint const& p1, CCPoint const& p2) const {
            return p1.fuzzyEquals(p2, .01f);
        }
    };
}

void AutoLinkManager::figureOutSet(CCArray* objs) {
    auto leftOver = objs->shallowCopy();
    std::unordered_map<CCPoint, std::vector<GameObject*>> gridSnapped;

    // Step 1: Figure out which objects are part of which grid part
    // To be considered part of the grid, the object must intersect a grid center
    for (auto obj : CCArrayExt<GameObject>(objs)) {
        auto rect = LevelEditorLayer::get()->getObjectRect(obj, false);
        // todo: fix if grid size is not 30.f
        auto pos = EditorUI::get()->getGridSnappedPos(obj->getPosition());
        if (rect.containsPoint(pos)) {
            leftOver->removeObject(obj);
            gridSnapped[pos].push_back(obj);
        }
    }

    // Step 2: Connect any left-over bits to their nearest object
    for (auto obj : CCArrayExt<GameObject>(leftOver)) {
        // If the bit is over 30 units away, don't include it
        float minDist = 30.f;
        std::vector<GameObject*>* target = nullptr;
        for (auto& [pt, tr] : gridSnapped) {
            auto dist = ccpDistance(obj->getPosition(), pt);
            if (dist < minDist) {
                minDist = dist;
                target = &tr;
            }
        }
        if (target) {
            target->push_back(obj);
        }
    }

    std::unordered_map<AutoLinkDefinition, std::vector<std::string>> definitions;
    // Step 3: Figure out template
    for (auto [pt, objs] : gridSnapped) {
        std::stringstream str;
        for (auto obj : objs) {
            // no recursive autolinking
            if (obj->m_objectID == AutoLinkObject::OBJ_ID) {
                continue;
            }
            str << std::string(obj->getSaveString()) << ";";
        }
        AutoLinkDefinition def;
        def.set(
            gridSnapped.contains(pt + CCPoint { 0.f, 30.f }),
            gridSnapped.contains(pt + CCPoint { 30.f, 0.f }),
            gridSnapped.contains(pt - CCPoint { 0.f, 30.f }),
            gridSnapped.contains(pt - CCPoint { 30.f, 0.f }),
            AutoLinkNeighbour::Block
        );
        definitions[def].push_back(str.str());
    }
    m_sets.push_back(AutoLinkSet::create(definitions));
}

AutoLinkManager* AutoLinkManager::get() {
    static auto ret = new AutoLinkManager;
    return ret;
}
