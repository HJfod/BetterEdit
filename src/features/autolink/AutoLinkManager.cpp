#include "AutoLinkManager.hpp"
#include <Geode/utils/cocos.hpp>

bool AutoLinkSet::init() {
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

AutoLinkSet* AutoLinkSet::create() {
    auto ret = new AutoLinkSet;
    if (ret && ret->init()) {
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
            str << std::string(obj->getSaveString()) << ";";
        }
        AutoLinkDefinition def;
    }
}

AutoLinkManager* AutoLinkManager::get() {
    static auto ret = new AutoLinkManager;
    return ret;
}
