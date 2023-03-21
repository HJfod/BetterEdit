#include "AutoLinkManager.hpp"

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

AutoLinkManager* AutoLinkManager::get() {
    static auto ret = new AutoLinkManager;
    return ret;
}
