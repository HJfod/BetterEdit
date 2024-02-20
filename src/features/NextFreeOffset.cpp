
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <utils/NextFreeOffsetInput.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

struct GroupIDAllocator {
    using ValueType = short;

    static constexpr short MIN_VALUE = 1;
    static constexpr short MAX_VALUE = 9999;

    static void init(std::span<GameObject*> const& objs, short upTo) {
        for (short v = 0; v < (upTo - 1); v += 1) {
            auto obj = objs[v / 10];
            if (v % 10 == 0) {
                obj->m_groups = new std::array<short, 10>();
                obj->m_groupCount = 0;
            }
            obj->m_groups->at(v % 10) = v + 1;
            obj->m_groupCount += 1;
        }
    }
    static void uninit(std::span<GameObject*> const& objs, short) {
        for (auto obj : objs) {
            delete obj->m_groups;
        }
    }
    static size_t getObjectCountToAllocate(short upTo) {
        // https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
        return upTo / 10 + (upTo % 10 != 0);
    }
};

class $modify(SetGroupIDLayer) {
    $override
    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        auto nextFreeMenu = m_mainLayer->getChildByID("next-free-menu");
        auto nextFreeOffset = NextFreeOffsetInput<GroupIDAllocator>::create();
        nextFreeOffset->setID("next-free-offset-input"_spr);
        nextFreeMenu->addChild(nextFreeOffset);
        nextFreeMenu->updateLayout();

        return true;
    }

    $override
    void onNextGroupID1(CCObject* sender) {
        auto _ = NextFreeOffsetInput<GroupIDAllocator>::addFakeObjects();
        SetGroupIDLayer::onNextGroupID1(sender);
    }
};
