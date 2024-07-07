
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <utils/NextFreeOffsetInput.hpp>
#include <utils/Warn.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

struct GroupIDSource final {
    using ValueType = short;

    static constexpr short MIN_VALUE = 1;
    static constexpr short MAX_VALUE = 9999;

    static void getUsedIDs(GameObject* obj, std::unordered_set<short>& used) {
        for (short i = 0; i < obj->m_groupCount; i += 1) {
            used.insert(obj->m_groups->at(i));
        }
        for (short i = 0; i < obj->m_colorGroupCount; i += 1) {
            used.insert(obj->m_colorGroups->at(i));
        }
        for (short i = 0; i < obj->m_opacityGroupCount; i += 1) {
            used.insert(obj->m_opacityGroups->at(i));
        }
        if (auto eobj = typeinfo_cast<EffectGameObject*>(obj)) {
            used.insert(eobj->m_centerGroupID);
            used.insert(eobj->m_targetGroupID);
            used.insert(eobj->m_activateGroup);
        }
    }
};

class $modify(SetGroupIDLayer) {
    $override
    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        auto nextFreeMenu = m_mainLayer->getChildByID("next-free-menu");
        auto nextFreeOffset = NextFreeOffsetInput<GroupIDSource>::create();
        nextFreeOffset->setID("next-free-offset-input"_spr);
        nextFreeMenu->addChild(nextFreeOffset);
        nextFreeMenu->updateLayout();

        return true;
    }

    $override
    void onNextGroupID1(CCObject*) {
        m_groupIDValue = NextFreeOffsetInput<GroupIDSource>::getNextFreeID();
        this->updateGroupIDLabel();

        // gammal allokerande version
        // auto _ = NextFreeOffsetInput<GroupIDAllocator>::addFakeObjects();
        // SetGroupIDLayer::onNextGroupID1(sender);
    }
};
