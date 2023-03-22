#pragma once

#include <Geode/binding/GameObject.hpp>
#include <Geode/loader/Mod.hpp>
#include "AutoLinkManager.hpp"

using namespace geode::prelude;

class AutoLinkObject : public GameObject {
protected:
    Ref<AutoLinkSet> m_set;
    bool m_slope = false;
    Ref<CCArray> m_objects = CCArray::create();
    std::string m_objectString = "";
    static inline std::vector<Ref<AutoLinkObject>> s_autoLinkObjects {};

    bool init(Ref<AutoLinkSet> set);

    void customObjectSetup(gd::map<gd::string, gd::string>& map) override;
    static void cleanAutoLinkObjects();

    AutoLinkNeighbour neighborage() const;

public:
    static AutoLinkObject* create(Ref<AutoLinkSet> set);
    CCArray* getObjects() const;

    void updateLinking();

    static constexpr size_t OBJ_ID = 5002;
};
