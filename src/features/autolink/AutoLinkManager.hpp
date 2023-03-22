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
    void set(bool up, bool right, bool down, bool left, AutoLinkNeighbour neigh) {
        if (left && up)         defs.upLeft = neigh;
        else if (up)            defs.up = neigh;
        else if (up && right)   defs.upRight = neigh;
        else if (right)         defs.right = neigh;
        else if (down && right) defs.downRight = neigh;
        else if (down)          defs.down = neigh;
        else if (down && left)  defs.downLeft = neigh;
        else if (left)          defs.left = neigh;
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

using Definitions = std::unordered_map<AutoLinkDefinition, std::vector<std::string>>;

class AutoLinkSet : public CCObject {
protected:
    Definitions m_definitions;

    bool init(Definitions const& defs);

public:
    static AutoLinkSet* create(Definitions const& defs);

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

