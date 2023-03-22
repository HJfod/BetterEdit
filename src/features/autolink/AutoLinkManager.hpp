#pragma once

#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

enum class AutoLinkNeighbour : unsigned char {
    None    = 0,
    Block   = 1,
    Slope   = 2,
};

union AutoLinkDefinition {
    struct {
        AutoLinkNeighbour upLeft : 4;
        AutoLinkNeighbour up : 4;
        AutoLinkNeighbour upRight : 4;
        AutoLinkNeighbour right : 4;
        AutoLinkNeighbour downRight : 4;
        AutoLinkNeighbour down : 4;
        AutoLinkNeighbour downLeft : 4;
        AutoLinkNeighbour left : 4;
    } defs;
    int value;
    AutoLinkDefinition() : value(0) {}
    void removeCorners() {
        defs.upLeft = AutoLinkNeighbour::None;
        defs.upRight = AutoLinkNeighbour::None;
        defs.downRight = AutoLinkNeighbour::None;
        defs.downLeft = AutoLinkNeighbour::None;
    }
    bool operator==(AutoLinkDefinition const& other) const {
        return value == other.value;
    }
};

namespace std {
    template <>
    struct std::hash<AutoLinkDefinition> {
        std::size_t operator()(AutoLinkDefinition const& def) const noexcept {
            return def.value;
        }
    };
}

class AutoLinkSet : public CCObject {
protected:
    std::unordered_map<AutoLinkDefinition, std::vector<std::string>> m_definitions;

    bool init();

public:
    static AutoLinkSet* create();

    std::string getObject(AutoLinkDefinition def);
};

class AutoLinkManager {
protected:
    std::vector<Ref<AutoLinkSet>> m_sets;

public:
    static AutoLinkManager* get();

    std::vector<Ref<AutoLinkSet>> getSets() const;
    void figureOutSet(CCArray* objs);
};

