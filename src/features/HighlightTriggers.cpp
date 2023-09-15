#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/EffectGameObject.hpp>

using namespace geode::prelude;

ccColor3B operator*(const ccColor3B& color, float mult) {
    return {
        static_cast<unsigned char>(static_cast<float>(color.r) * mult),
        static_cast<unsigned char>(static_cast<float>(color.g) * mult),
        static_cast<unsigned char>(static_cast<float>(color.b) * mult)
    };
}

ccColor3B operator+(const ccColor3B& a, const ccColor3B& b) {
    return ccc3(
        a.r + b.r,
        a.g + b.g,
        a.b + b.b
    );
}

class ObjectPulseAction : public CCActionInterval {
public:
    GameObject* m_target;
    ccColor3B m_start, m_color;
    bool init(float d, const ccColor3B dest) {
        if (!CCActionInterval::initWithDuration(d)) return false;
        m_color = dest;
        return true;
    }
    virtual void startWithTarget(CCNode* target) {
        CCActionInterval::startWithTarget(target);
        if (auto rgb = dynamic_cast<CCNodeRGBA*>(target)) {
            m_start = rgb->getColor();
            m_target = reinterpret_cast<decltype(m_target)>(rgb);
        } else {
            m_target = nullptr;
        }
    }
    virtual void step(float time) {
        CCActionInterval::step(time);
        if (m_target) {
            const float it = 1.f - time;
            m_target->selectObject(m_start * time + m_color * it);
        }
    }
    virtual void isDone() override {
        CCActionInterval::isDone();
        if (m_target) {
            m_target->selectObject({255, 255, 255});
            m_target->m_bIsSelected = false;
        }
    }
    static auto create(float d, const ccColor3B dest) {
        auto ret = new ObjectPulseAction;
        if (ret && ret->init(d, dest)) {
            ret->autorelease();
        } else
            CC_SAFE_RELEASE_NULL(ret);
        return ret;
    }
};

class $modify(EffectGameObject) {
    void triggerObject(GJBaseGameLayer* layer) {
        EffectGameObject::triggerObject(layer);

        constexpr int TAG = 400123;
        // this doesnt even work properly, i hate cocos
        if (auto action = self->getActionByTag(TAG)) {
            self->stopAction(action);
        }
        auto action = ObjectPulseAction::create(.3f, {255, 0, 0});
        action->setTag(TAG);
        self->runAction(action);
    }
};